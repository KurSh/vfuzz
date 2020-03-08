#pragma once

#include <mutator/ranged.h>
#include <util/string/string.h>

namespace vfuzz {
namespace mutator {

template <bool isRanged>
class And : public DefaultRanged<isRanged, true> {
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
        std::string desc(const uint8_t modifier) override;
    public:
        And(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class Or : public DefaultRanged<isRanged, true> {
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
        std::string desc(const uint8_t modifier) override;
    public:
        Or(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class Xor : public DefaultRanged<isRanged, true> {
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
        std::string desc(const uint8_t modifier) override;
    public:
        Xor(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class Not : public DefaultRanged<isRanged, false> {
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
        std::string desc(const uint8_t modifier) override;
    public:
        Not(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class ShiftLeft : public DefaultRanged<isRanged, true> {
    private:
        uint8_t adjustModifier(const uint8_t modifier);
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
        std::string desc(const uint8_t modifier) override;
    public:
        ShiftLeft(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class ShiftRight : public DefaultRanged<isRanged, true> {
    private:
        uint8_t adjustModifier(const uint8_t modifier);
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
        std::string desc(const uint8_t modifier) override;
    public:
        ShiftRight(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class ChangeBit : public DefaultRanged<isRanged, true> {
    private:
        uint8_t adjustModifier(const uint8_t modifier);
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
        std::string desc(const uint8_t modifier) override;
    public:
        ChangeBit(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class ChangeByte : public DefaultRanged<isRanged, true> {
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
        std::string desc(const uint8_t modifier) override;
    public:
        ChangeByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class ReverseByte : public DefaultRanged<isRanged, false> {
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
    public:
        ReverseByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class RORByte : public DefaultRanged<isRanged, true> {
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
    public:
        RORByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

template <bool isRanged>
class ROLByte : public DefaultRanged<isRanged, true> {
    protected:
        uint8_t process(const uint8_t in, const uint8_t modifier) override;
    public:
        ROLByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

} /* namespace mutator */
} /* namespace vfuzz */
