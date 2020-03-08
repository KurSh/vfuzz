#include <mutator/history.h>
#include <boost/algorithm/string/join.hpp>
#include <sstream>

namespace vfuzz {
namespace mutator {

void History::Add(const GeneratorID gid, const std::string name, const std::string desc) {
    history[gid].push_back( std::make_pair(name, desc) );
}

void History::Clear(void) {
    history.clear();
}

std::string History::ToString(void) const {
    std::vector<std::string> parts;

    for ( const auto& kv : history ) {
        std::vector<std::string> parts_inner;

        const GeneratorID& gid = kv.first;
        const std::vector<std::pair<std::string, std::string>>& v = kv.second;

        for ( const auto& info : v ) {
            std::stringstream ss;
            ss << info.first; /* name */
            if ( info.second.empty() == false ) { /* desc. */
                ss << "(" << info.second << ")";
            }
            parts_inner.push_back( ss.str() );
        }

        if ( parts_inner.empty() == false) {
            std::stringstream ss;
            ss << "GID #" << gid << ": " << boost::algorithm::join(parts_inner, "-");
            parts.push_back( ss.str() );
        }
    }

    return boost::algorithm::join(parts, "\n");
}

} /* namespace mutator */
} /* namespace vfuzz */
