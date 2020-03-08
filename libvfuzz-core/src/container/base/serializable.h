#pragma once

namespace vfuzz {

class Serializable {
    public:
        virtual std::vector<uint8_t> Serialize(void) const = 0;
        virtual void Deserialize(const std::vector<uint8_t> serialized) = 0;
        Serializable() = default;
        virtual ~Serializable() = default;
};

} /* namespace vfuzz */
