#include <mutator/range_based.h>

namespace vfuzz {
namespace mutator {

template <bool isRanged>
uint8_t And<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    return in & modifier;
}

template <bool isRanged>
std::string And<isRanged>::desc(const uint8_t modifier) {
    return util::string::ToHexString(modifier);
}

template <bool isRanged>
And<isRanged>::And(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, true>(Rand, "And", history) {
}

template <bool isRanged>
uint8_t Or<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    return in | modifier;
}

template <bool isRanged>
std::string Or<isRanged>::desc(const uint8_t modifier) {
    return util::string::ToHexString(modifier);
}

template <bool isRanged>
Or<isRanged>::Or(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, true>(Rand, "Or", history) {
}

template <bool isRanged>
uint8_t Xor<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    return in ^ modifier;
}

template <bool isRanged>
std::string Xor<isRanged>::desc(const uint8_t modifier) {
    return util::string::ToHexString(modifier);
}

template <bool isRanged>
Xor<isRanged>::Xor(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, true>(Rand, "Xor", history) {
}

template <bool isRanged>
uint8_t Not<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    (void)modifier;
    return ~in;
}

template <bool isRanged>
std::string Not<isRanged>::desc(const uint8_t modifier) {
    return util::string::ToHexString(modifier);
}

template <bool isRanged>
Not<isRanged>::Not(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, false>(Rand, "Not", history) {
}

template <bool isRanged>
uint8_t ShiftLeft<isRanged>::adjustModifier(const uint8_t modifier) {
    return modifier & 7 ? (modifier & 7) : 1;
}

template <bool isRanged>
uint8_t ShiftLeft<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    return in << adjustModifier(modifier);
}

template <bool isRanged>
std::string ShiftLeft<isRanged>::desc(const uint8_t modifier) {
    return std::to_string(adjustModifier(modifier));
}

template <bool isRanged>
ShiftLeft<isRanged>::ShiftLeft(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, true>(Rand, "ShiftLeft", history) {
}

template <bool isRanged>
uint8_t ShiftRight<isRanged>::adjustModifier(const uint8_t modifier) {
    return modifier & 7 ? (modifier & 7) : 1;
}

template <bool isRanged>
uint8_t ShiftRight<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    return in >> adjustModifier(modifier);
}

template <bool isRanged>
std::string ShiftRight<isRanged>::desc(const uint8_t modifier) {
    return std::to_string(adjustModifier(modifier));
}

template <bool isRanged>
ShiftRight<isRanged>::ShiftRight(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, true>(Rand, "ShiftRight", history) {
}

template <bool isRanged>
uint8_t ChangeBit<isRanged>::adjustModifier(const uint8_t modifier) {
    return (1 << (modifier & 7));
}

template <bool isRanged>
uint8_t ChangeBit<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    return in ^ adjustModifier(modifier);
}

template <bool isRanged>
std::string ChangeBit<isRanged>::desc(const uint8_t modifier) {
    return util::string::ToBinString(adjustModifier(modifier));
}

template <bool isRanged>
ChangeBit<isRanged>::ChangeBit(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, true>(Rand, "ChangeBit", history) {
}

template <bool isRanged>
uint8_t ChangeByte<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    (void)in;
    return modifier;
}

template <bool isRanged>
std::string ChangeByte<isRanged>::desc(const uint8_t modifier) {
    return util::string::ToHexString(modifier);
}

template <bool isRanged>
ChangeByte<isRanged>::ChangeByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, true>(Rand, "ChangeByte", history) {
}

template <bool isRanged>
uint8_t ReverseByte<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    (void)modifier;
    return __builtin_bitreverse8(in);
}

template <bool isRanged>
ReverseByte<isRanged>::ReverseByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, false>(Rand, "ReverseByte", history) {
}

template <bool isRanged>
uint8_t RORByte<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    return __builtin_rotateright8(in, modifier);
}

template <bool isRanged>
RORByte<isRanged>::RORByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, true>(Rand, "RORByte", history) {
}

template <bool isRanged>
uint8_t ROLByte<isRanged>::process(const uint8_t in, const uint8_t modifier) {
    return __builtin_rotateleft8(in, modifier);
}

template <bool isRanged>
ROLByte<isRanged>::ROLByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    DefaultRanged<isRanged, true>(Rand, "ROLByte", history) {
}

/* Instantiation */
template class And<false>;
template class And<true>;
template class Or<false>;
template class Or<true>;
template class Xor<false>;
template class Xor<true>;
template class Not<false>;
template class Not<true>;
template class ShiftLeft<false>;
template class ShiftLeft<true>;
template class ShiftRight<false>;
template class ShiftRight<true>;
template class ChangeBit<false>;
template class ChangeBit<true>;
template class ChangeByte<false>;
template class ChangeByte<true>;
template class ReverseByte<false>;
template class ReverseByte<true>;
template class RORByte<false>;
template class RORByte<true>;
template class ROLByte<false>;
template class ROLByte<true>;

} /* namespace mutator */
} /* namespace vfuzz */
