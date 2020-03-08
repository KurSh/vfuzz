#include <datasource/datasource.h>
#include <exec/executor.h>

vfuzz::Executor* vfuzz::g_executor = nullptr;

namespace vfuzz {
namespace datasource {

Datasource::Datasource(void) :
    Base()
{
}

std::vector<uint8_t> Datasource::get(const size_t min, const size_t max, const uint64_t id)
{
    uint8_t* data;
    size_t size;
    vfuzz::g_executor->GetNextChunk(id, &data, &size, min, max);
    return std::vector<uint8_t>(data, data + size);
}

} /* namespace datasource */
} /* namespace vfuzz */
