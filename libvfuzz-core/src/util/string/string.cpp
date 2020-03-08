#include <util/string/string.h>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/algorithm/hex.hpp>
#include <sstream>
#include <string>
#include <bitset>

namespace vfuzz {
namespace util {
namespace string {

void RemoveWhitespace(std::string& in)
{
    in.erase(std::remove_if(in.begin(), in.end(), ::isspace), in.end());
}

void ToLowercase(std::string& in)
{
    std::transform(in.begin(), in.end(), in.begin(), ::tolower);
}

std::vector<std::string> Split(const std::string& in, const std::string delimiter)
{
    std::vector<std::string> parts;

    boost::split(parts, in, boost::is_any_of(delimiter));

    return parts;
}

/* Adapted from https://stackoverflow.com/questions/7053538/how-do-i-encode-a-string-to-base64-using-only-boost */
std::string ToBase64(const uint8_t* data, const size_t size)
{
    using namespace boost::archive::iterators;

    std::stringstream os;
    typedef
        base64_from_binary<
            transform_width<
                const char *,
                6,
                8
            >
        > base64_text;

    std::copy(
        base64_text(data),
        base64_text(data + size),
        ostream_iterator<char>(os)
    );

    return os.str();
}

std::string ToHexString(Chunk chunk, const size_t maxLen) {
    const bool isTooLarge = chunk.size() > maxLen;
    if ( isTooLarge ) {
        chunk.resize(maxLen);
    }
    std::string ret;
    boost::algorithm::hex(chunk, std::back_inserter(ret));
    if ( isTooLarge ) {
        ret += "..";
    }
    return ret;
}

std::string ToHexString(const uint8_t c) {
    std::string ret;
    Chunk chunk(&c, &c + sizeof(c));
    boost::algorithm::hex(chunk, std::back_inserter(ret));
    return ret;
}

std::string ToBinString(const uint8_t c) {
    return std::bitset<8>(c).to_string();
}

} /* namespace string */
} /* namespace util */
} /* namespace vfuzz */
