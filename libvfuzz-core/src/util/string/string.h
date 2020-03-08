#pragma once

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <base/types.h>

namespace vfuzz {
namespace util {
namespace string {

void RemoveWhitespace(std::string& in);
void ToLowercase(std::string& in);
std::vector<std::string> Split(const std::string& in, const std::string delimiter);
std::string ToBase64(const uint8_t* data, const size_t size);
std::string ToHexString(Chunk chunk, const size_t maxLen = 8);
std::string ToHexString(const uint8_t c);
std::string ToBinString(const uint8_t c);

} /* namespace string */
} /* namespace util */
} /* namespace vfuzz */
