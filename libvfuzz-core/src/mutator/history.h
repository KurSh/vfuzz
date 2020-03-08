#pragma once

#include <base/types.h>
#include <map>
#include <vector>
#include <utility>
#include <string>

namespace vfuzz {
namespace mutator {

class History {
    private:
        std::map< const GeneratorID, std::vector< std::pair<std::string, std::string> > > history;
    public:
        void Add(const GeneratorID gid, const std::string name, const std::string desc);
        void Clear(void);
        std::string ToString(void) const;
};

} /* namespace mutator */
} /* namespace vfuzz */
