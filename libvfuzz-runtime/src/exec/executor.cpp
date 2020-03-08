#include <iostream>
#include <exec/executor.h>
#include <base/exception.h>
#include <mutator/util.h>
#include <interface/extfunctions.h>
#include <util/mallocfreetracer.h>
#include <util/io.h>
#include <sensor/sensor/base.h>
#include <cassert>

static GF_ExternalFunctions *GF_EF = nullptr;
static bool HaveLeakSanitizer = false;

MallocFreeTracer AllocTracer;
void MallocHook(const volatile void *ptr, size_t size) {
    (void)ptr;
    (void)size;

    AllocTracer.AddMalloc();
    /* TODO TODO TODO */
    //Sensor::MallocHook(ptr, size);
}

void FreeHook(const volatile void *ptr) {
    (void)ptr;

    AllocTracer.AddFree();
    /* TODO TODO TODO */
    //Sensor::FreeHook(ptr);
}

namespace vfuzz {

Executor::Executor(
        std::shared_ptr<util::Random> Rand,
        util::PushPullCallback<std::shared_ptr<container::InputCluster>> icPushPullCB,
        std::shared_ptr<mutator::Container> mutators,
        const size_t maxChunks,
        const size_t maxChunkLen,
        const size_t maxMutations,
        std::optional<std::shared_ptr<dictionary::Dictionary>> dictionary,
        std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    icPushPullCB(icPushPullCB),
    mutators(mutators),
    Rand(Rand),
    sensorDriver(std::make_shared<sensor::Driver>(loggerChain)),
    maxChunks(maxChunks),
    maxChunkLen(maxChunkLen),
    maxMutations(maxMutations),
    loggerChain(loggerChain),
    dictionary(dictionary)
{
}

void Executor::GetNextChunk(const GeneratorID gid, uint8_t **const data, size_t *const size, size_t min, size_t max) {
    {
        /* Curtail to the globally defined limit */

        if ( max == 0 ) {
            max = maxChunkLen;
        }

        max = std::min(max, maxChunkLen);
        min = std::min(min, max);

        assert(min && max ? min <= max : true);
        assert(max ? max <= maxChunkLen : true);
    }

    if ( gidToCurrentIndex.count(gid) == 0 ) {
        /* The first time this generator is requested */
        gidToCurrentIndex[gid] = 0;
    }

    /* From the selected input cluster, retrieve the input for this gid */
    auto input = curInputCluster->Get(gid);

    assert(input != nullptr);

    /* Determine which chunk index we want */
    const container::ChunkIndex chunkIndex = gidToCurrentIndex[gid];

    if ( mode == ExecutorModeDynamic ) {
        /* Mutate a random existing chunk */

        //const size_t numMutations = Rand(1, maxMutations);
        const size_t numMutations = Rand->Get(0, maxMutations);
        for (size_t i = 0; i < numMutations; i++) {
            input->MutateChunk(gid, chunkIndex, mutators->GetRandomMutator(), max);
            assert( input->Get(chunkIndex).size() <= maxChunkLen );
        }
    }

    /* Crop and copy chunk to user */
    {
        if ( min > 0 ) {
            input->ExpandChunk(chunkIndex, min);
        }

        if ( max > 0 ) {
            input->CropChunk(chunkIndex, max);
        }


        const Chunk chunk = input->Get(chunkIndex);

        if ( chunk.size() < min || chunk.size() > max ) {
            abort();
        }

        /* Copy chunk data to the caller */
        *data = (uint8_t*)malloc( chunk.size() );
        *size = chunk.size();
        if ( *size > 0 ) {
            memcpy(*data, chunk.data(), *size);
        }

        /* Must be freed by us after this cycle */
        toFree.push_back(*data);
    }

    haveGenerated = true;

    if ( gidToCurrentIndex[gid] < maxChunks ) {
        gidToCurrentIndex[gid]++;
    } else {
        /* TODO Find a more elegant solution. */
        throw vfuzz::Exception("More chunks requested than max-chunks allows");
    }
}

void Executor::GetNextChunk(const GeneratorID gid, uint8_t **const data, size_t *const size) {
    GetNextChunk(gid, data, size, 0, 0);
}

void Executor::StaticDeathCallback() {
    g_executor->HandleCrash("crash", false);
    if ( g_executor->loggerChain ) {
        (*(g_executor->loggerChain))->Stop();
    }
}

void Executor::HandleCrash(const std::string prefix, const bool doExit)
{
    if ( g_executor->haveGenerated == false ) {
        if ( g_executor->loggerChain ) {
            (*(g_executor->loggerChain))->AddInfo(Logger::LOG_LEVEL_WARNING, "target crashed without consuming any input");
        }
        goto end;
    }

    if ( g_executor->onCrash ) {
        if ( g_executor->loggerChain ) {
            (*(g_executor->onCrash))(prefix, g_executor->curInputCluster, doExit);
        }
    }

end:
    if ( !g_executor->onCrash && doExit == true ) {
        if ( g_executor->loggerChain ) {
            (*(g_executor->loggerChain))->Stop();
        }
        _Exit(1);
    }
}

void Executor::Initialize(int argc, char** argv)
{
    /* Detect external functions */
    {
        GF_EF = new GF_ExternalFunctions();
        //HaveLeakSanitizer = &(GF_EF->__lsan_enable) && &(GF_EF->__lsan_disable) && (GF_EF->__lsan_do_recoverable_leak_check);
        HaveLeakSanitizer = false;

        if (GF_EF->__sanitizer_install_malloc_and_free_hooks)
            GF_EF->__sanitizer_install_malloc_and_free_hooks(MallocHook, FreeHook);
        if (GF_EF->__sanitizer_set_death_callback)
            GF_EF->__sanitizer_set_death_callback(StaticDeathCallback);
    }

    sensorDriver->StartRecord();
    if (GF_EF->VFuzzInit) {
        GF_EF->VFuzzInit(argc, argv);
    }
    sensorDriver->StopRecord();
}

void Executor::Run(void) {
    setModeDynamic();

    prepare();

    if ( HaveLeakSanitizer == true ) {
        AllocTracer.Start();
    }

    curInputCluster = icPushPullCB.Pull();
    VFuzzRun();

    if ( HaveLeakSanitizer == true ) {
        const bool HasMoreMallocsThanFrees = AllocTracer.Stop();
        if ( HasMoreMallocsThanFrees == true ) {
            if (GF_EF->__lsan_do_recoverable_leak_check()) {
                std::cout << "Memory leak detected" << std::endl;
                HandleCrash("leak", true);
            }
        }
    }

    const bool haveSensorIncrease = finish();

    if ( doDiscard == false && haveSensorIncrease == true ) {
        if ( onNew ) {
            (*onNew)(curInputCluster);
        }
        icPushPullCB.Push(curInputCluster);
        curInputCluster = nullptr;
    } else {
        curInputCluster = nullptr;
    }
}

void Executor::prepare(void) {
    prepare(sensorDriver);
}

void Executor::prepare(std::shared_ptr<sensor::Driver> sensorDriver) {
    /* TODO only clear after a period of time. Or use a rolling dictionary */
    if ( dictionary ) {
        (*dictionary)->ClearSet(dictionary::kDictionaryTypeMemcmp);
        (*dictionary)->ClearSet(dictionary::kDictionaryTypeCmp);
    }
    sensorDriver->StartRecord();
    doDiscard = false;
    haveGenerated = false;
    g_executor = this;

}

bool Executor::finish(void) {
    return finish(sensorDriver);
}

bool Executor::finish(std::shared_ptr<sensor::Driver> sensorDriver) {
    gidToCurrentIndex.clear();
    for ( const auto& f : toFree ) {
        free(f);
    }
    toFree.clear();

    g_executor = nullptr;
    const bool haveSensorIncrease = sensorDriver->StopRecord();

    return haveSensorIncrease;
}

bool Executor::RunInputCluster(std::shared_ptr<container::InputCluster> inputCluster) {
    setModeStatic();
    curInputCluster = inputCluster;

    prepare();
    VFuzzRun();
    const bool haveSensorIncrease = finish();
    if ( doDiscard == false && haveSensorIncrease == true ) {
        icPushPullCB.Push(std::make_shared<container::InputCluster>(*inputCluster));
    }

    return haveSensorIncrease;
}

sensor::Data Executor::InputClusterToSensorData(std::shared_ptr<container::InputCluster> inputCluster) {
    setModeStatic();
    curInputCluster = inputCluster;

    auto sensorDriver = std::make_shared<sensor::Driver>();

    prepare(sensorDriver);
    VFuzzRun();
    finish(sensorDriver);

    sensor::Data ret = sensorDriver->GetSensorData();

    return ret;
}

void Executor::AddReplace(std::pair<Chunk, Chunk> replacePair) {
    if ( recordReplace == true ) {
        if ( replacePair.first != replacePair.second ) {
            replacings.insert( replacePair );
        }
    }
}

void Executor::SetDiscard(const bool _doDiscard)
{
    doDiscard = _doDiscard;

    if ( doDiscard == true ) {
        if ( loggerChain ) {
            (*loggerChain)->SetAutoFlush(false);
        }
        /* TODO disable sensors ? */
    } else {
        if ( loggerChain ) {
            (*loggerChain)->SetAutoFlush(true);
        }
    }
}

void Executor::SetOnNew(const std::function<void(const std::shared_ptr<container::InputCluster> inputCluster)> _onNew)
{
    onNew = _onNew;
}

void Executor::SetOnCrash(const std::function<void(const std::string prefix, const std::shared_ptr<container::InputCluster> inputCluster, const bool doExit)> _onCrash)
{
    onCrash = _onCrash;
}

extern "C" void vfuzz_discard(void) {
    if ( vfuzz::g_executor == nullptr ) {
        /* TODO throw ? */
        return;
    }

    vfuzz::g_executor->SetDiscard(true);
}

extern "C" void vfuzz_dont_discard(void) {
    if ( vfuzz::g_executor == nullptr ) {
        /* TODO throw ? */
        return;
    }

    vfuzz::g_executor->SetDiscard(false);
}

extern "C" void vfuzz_crash(void)
{
    if ( vfuzz::g_executor == nullptr ) {
        /* TODO throw ? */
        return;
    }
    vfuzz::g_executor->StaticDeathCallback();
    exit(0);
}

/* TODO maybe put this in a different class */
container::InputCluster Executor::ShrinkInputCluster(std::shared_ptr<container::InputCluster> inputCluster) {
    auto prevSensorData = InputClusterToSensorData(inputCluster);
    {
        auto inputClusterCopy = *inputCluster;
    }

    return *inputCluster;
}

} /* namespace vfuzz */
