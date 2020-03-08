#include <util/io.h>
#include <base/exception.h>
#include <cstdio>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <util/sha1.h>

namespace vfuzz {
namespace util {

std::optional<std::vector<uint8_t>> FileToVector(const std::string &path, const size_t maxsize) {
  std::vector<uint8_t> ret;
  std::ifstream T(path);
  if ( !T ) {
      return {};
  }

  T.seekg(0, T.end);
  size_t filesize = T.tellg();
  if (maxsize)
    filesize = std::min(filesize, maxsize);

  T.seekg(0, T.beg);
  ret.resize(filesize);
  T.read(reinterpret_cast<char *>(ret.data()), filesize);

  return {ret};
}

bool IsDirectory(const std::string &path)
{
  struct stat st;
  if ( stat(path.c_str(), &st) ) {
      return false;
  }

  return S_ISDIR(st.st_mode);
}

std::string DirPlusFile(const std::string &dirpath,
                        const std::string &filename) {
    return
        boost::trim_right_copy_if(dirpath, boost::is_any_of("/"))
        + "/" + filename;
}

std::vector<std::string> FilesInDirectory(const std::string &path)
{
    std::vector<std::string> ret;

    if ( IsDirectory(path) == false ) {
        throw vfuzz::Exception("Unexpected: " + path + " is not a directory");
    }

    DIR *D = opendir(path.c_str());

    /* TODO deal with D == NULL */

    while ( auto E = readdir(D) ) {
        std::string fn = DirPlusFile(path, E->d_name);
        if (E->d_type == DT_REG || E->d_type == DT_LNK)
            ret.push_back(fn);
    }

    closedir(D);

    return ret;
}

void WriteToFile(const std::vector<uint8_t> &U, const std::string &Path) {
  FILE *Out = fopen(Path.c_str(), "w");
  if (!Out) return;
  fwrite(U.data(), sizeof(U[0]), U.size(), Out);
  fclose(Out);
}

void WriteInputCluster(const std::string path, const std::string prefix, const std::shared_ptr<container::InputCluster> inputCluster, std::shared_ptr<LoggerChain> loggerChain)
{
    const auto serialized = inputCluster->Serialize();
    const auto hash = Hash(serialized);

    const auto ext_prefix = prefix + (prefix.size() > 0 ? "-" : "");

    const auto filename = DirPlusFile(path, ext_prefix + hash);

    loggerChain->AddInfo(Logger::LOG_LEVEL_INFO, "Writing " + filename);
    WriteToFile(serialized, filename);
}

} /* namespace util */
} /* namespace vfuzz */
