#include <container/input.h>
#include <base/exception.h>
#include <cstring>

namespace vfuzz {
namespace container {

Input::Input(std::shared_ptr<util::Random> Rand) :
    Rand(Rand) {
}

Input::Input(const Input &input) {
    for ( const auto& M : input.mutators ) {
        mutators.insert(M);
    }
    for ( const auto& C : input.chunks ) {
        chunks.push_back(C);
    }
    Rand = input.Rand;
}

Chunk Input::Get(const ChunkIndex chunkIndex) const {
    if ( chunkIndex >= chunks.size() ) {
        /* If it doesn't exist, generate a new chunk */
        /* TODO Add(Chunk()) ? */
        return Chunk();
    }
    return chunks[chunkIndex];
}

Chunk Input::GetRandom(void) const {
    if ( chunks.empty() == true ) {
        return Chunk();
    }
    return Get(Rand->Get(chunks.size() - 1));
}

void Input::Add(const Chunk chunk) {
    chunks.push_back(chunk);
}

void Input::Set(const ChunkIndex chunkIndex, const Chunk chunk) {
    if ( chunkIndex >= chunks.size() ) {
        /* TODO check if within max # of chunks */
        chunks.push_back(chunk);
        return;
    }
    chunks[chunkIndex] = chunk;
}

size_t Input::Size(void) const {
    return chunks.size();
}

void Input::ExpandChunk(const ChunkIndex chunkIndex, const size_t min)
{
    Chunk chunk = Get(chunkIndex);
    if ( chunk.size() < min ) {
        while ( chunk.size() < min ) {
            chunk.push_back( Rand->Get(256) );
        }
        Set(chunkIndex, chunk);
    }
}

/* If the chunk at 'chunkIndex' is larger than 'max', resize it to 'max' */
void Input::CropChunk(const ChunkIndex chunkIndex, const size_t max) {
    /* TODO test */
    Chunk chunk = Get(chunkIndex);
    if ( chunk.size() > max ) {
        chunk.resize(max);
        Set(chunkIndex, chunk);
    }
}

std::shared_ptr<Input> Input::MutateRandom(const GeneratorID gid, std::shared_ptr<mutator::Mutator> mutator, const size_t maxChunks, const size_t maxChunkLen) {
    std::shared_ptr<Input> ret = shared_from_this();
    bool addChunk;

    if ( ret->Size() == 0 ) {
        /* No chunks? Add one */
        addChunk = true;
    } else if (Rand->RandBool() == true && ret->Size() < maxChunks) {
        /* Fewer chunks than maxChunks? Add one */
        addChunk = true;
    } else {
        addChunk = false;
    }

    if ( addChunk ) {
        ret->Add(Chunk());
    }

    /* Pick a random chunk */
    const ChunkIndex chunkIndex = Rand->Get(ret->Size());
    /* Mutate this chunk */
    ret->MutateChunk(gid, chunkIndex, mutator, maxChunkLen);
    /*
    Chunk chunk = ret->Get(idx);
    Chunk chunkMutated = mutator->Mutate(chunk);
    if ( chunkMutated.size() > maxChunkLen ) {
        ret->Set(idx, chunk);
    } else {
        mutators.insert(mutator);
        ret->Set(idx, chunkMutated);
    }
    */
    return ret;
}

/* Mutate a specific chunk */
void Input::MutateChunk(const GeneratorID gid, const size_t chunkIdx, std::shared_ptr<mutator::Mutator> mutator, const size_t maxChunkLen) {
    /* TODO give mutator access to rest of corpus ? */
    const Chunk chunk = Get(chunkIdx);
    Chunk insert;
    const Chunk chunkMutated = mutator->Mutate(gid, chunk, insert, maxChunkLen);

    if ( chunkMutated.size() > maxChunkLen ) {
        /* TODO do nothing ? chunk = Crop(chunk); ? */

        Set(chunkIdx, chunk);
    } else {
        mutators.insert(mutator);

        if ( insert.empty() == false ) {
            insertions.push_back(insert);
        }

        Set(chunkIdx, chunkMutated);
    }
}

void Input::ClearMutators(void) {
    mutators.clear();
}

void Input::ClearInsertions(void) {
    insertions.clear();
}

std::set<std::shared_ptr<mutator::Mutator>> Input::GetMutators(void) const {
    return mutators;
}

std::vector<Chunk> Input::GetInsertions(void) const {
    return insertions;
}

std::vector<uint8_t> Input::Serialize(void) const {
    std::vector<uint8_t> ret;

    /* Calculate and allocate required size */
    {
        size_t totalsize = 0;

        totalsize += sizeof(uint64_t);
        for ( const auto& C : chunks ) {
            totalsize += sizeof(uint64_t);
            totalsize += C.size();
        }

        ret.resize(totalsize);
    }

    size_t pos = 0;
    const uint64_t total_chunks = chunks.size();

    memcpy(&(ret[pos]), &total_chunks, sizeof(total_chunks));

    pos += sizeof(total_chunks);

    for ( const auto& C : chunks ) {
        uint64_t chunk_size = C.size();

        memcpy(&(ret[pos]), &chunk_size, sizeof(chunk_size));
        pos += sizeof(chunk_size);

        if ( C.empty() == false ) {
            memcpy(&(ret[pos]), C.data(), C.size());
            pos += C.size();
        }
    }

    return ret;
}

void Input::Deserialize(const std::vector<uint8_t> serialized) {
    static const std::string deserialize_error("Input deserialization: input data corrupted");

    size_t pos = 0;
    size_t size_left = serialized.size();

    uint64_t total_chunks;
    /* Deserialize total_chunks */
    {
        if ( size_left < sizeof(total_chunks) ) {
            throw DeserializationException();
        }

        memcpy(&total_chunks, &(serialized[pos]), sizeof(total_chunks));
        pos += sizeof(total_chunks);
        size_left -= sizeof(total_chunks);
    }

    for (size_t i = 0; i < total_chunks; i++) {
        uint64_t chunk_size;
        /* Deserialize chunk_size */
        {
            if ( size_left < sizeof(chunk_size) ) {
                throw DeserializationException();
            }

            memcpy(&chunk_size, &(serialized[pos]), sizeof(chunk_size));
            pos += sizeof(chunk_size);
            size_left -= sizeof(chunk_size);
        }

        if ( size_left < chunk_size ) {
            throw DeserializationException();
        }

        /* Deserialize chunk */
        {
            if ( chunk_size > 0 ) {
                Chunk chunk(chunk_size);

                memcpy(chunk.data(), &(serialized[pos]), chunk_size);

                pos += chunk_size;
                size_left -= chunk_size;

                this->Set(i, chunk);
            } else {
                this->Set(i, {});
            }
        }

    }
}

size_t Input::GetLargestChunkSize(void) const {
    size_t largestChunkSize = 0;

    for ( const auto& C : chunks ) {
        if ( C.size() > largestChunkSize ) {
            largestChunkSize = C.size();
        }
    }

    return largestChunkSize;
}

} /* namespace container */
} /* namespace vfuzz */
