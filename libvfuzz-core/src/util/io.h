#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <container/inputcluster.h>
#include <logger/loggerchain.h>

namespace vfuzz {
namespace util {

std::optional<std::vector<uint8_t>> FileToVector(const std::string &path, const size_t maxsize);
std::string DirPlusFile(const std::string &dirpath, const std::string &filename);
bool IsDirectory(const std::string &path);
std::vector<std::string> FilesInDirectory(const std::string &path);
void WriteToFile(const std::vector<uint8_t> &U, const std::string &Path);
void WriteInputCluster(const std::string path, const std::string prefix, const std::shared_ptr<container::InputCluster> inputCluster, std::shared_ptr<LoggerChain> loggerChain);

} /* namespace util */
} /* namespace vfuzz */
