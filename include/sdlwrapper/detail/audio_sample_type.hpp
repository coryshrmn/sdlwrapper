#ifndef SDLWRAPPER_DETAIL_AUDIO_SAMPLE_TYPE_HPP
#define SDLWRAPPER_DETAIL_AUDIO_SAMPLE_TYPE_HPP

#include <climits>

#include <boost/integer.hpp>

namespace sdlwrapper
{
namespace detail
{

// specialize FloatStruct for each floating type (float, double, long double)
template <int bits, typename = void>
struct FloatStruct;

template <>
struct FloatStruct<sizeof(float) * CHAR_BIT>
{
    using Type = float;
};

template <>
struct FloatStruct<sizeof(double) * CHAR_BIT, std::enable_if_t<sizeof(float) != sizeof(double)>>
{
    using Type = double;
};

template <>
struct FloatStruct<sizeof(long double) * CHAR_BIT, std::enable_if_t<sizeof(double) != sizeof(long double)>>
{
    using Type = long double;
};

// we use Boost Integer instead of specializing each Int/UInt

// determine the sample type from format parameters
template <bool isFloating, int bits, bool isSigned = false>
struct AudioSampleTypeStruct;

template <bool isFloating, int bits, bool isSigned = false>
using AudioSampleType = typename AudioSampleTypeStruct<isFloating, bits, isSigned>::Type;

// floating
template <int bits, bool isSigned>
struct AudioSampleTypeStruct<true, bits, isSigned>
{
    using Type = typename FloatStruct<bits>::Type;
};

// unsigned integral
template <int bits>
struct AudioSampleTypeStruct<false, bits, false>
{
    using Type = typename boost::uint_t<bits>::exact;
};

// signed integral
template <int bits>
struct AudioSampleTypeStruct<false, bits, true>
{
    using Type = typename boost::int_t<bits>::exact;
};

} // namespace detail
} // namespace sdlwrapper

#endif // SDLWRAPPER_DETAIL_AUDIO_SAMPLE_TYPE_HPP
