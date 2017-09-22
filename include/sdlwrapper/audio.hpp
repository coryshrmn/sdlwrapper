#ifndef SDLWRAPPER_AUDIO_HPP
#define SDLWRAPPER_AUDIO_HPP

#include "sdlwrapper/sdl.hpp"
#include "sdlwrapper/detail/audio_sample_type.hpp"

#include <boost/integer.hpp>

namespace sdlwrapper
{

using AudioFormat = SDL_AudioFormat;

template <AudioFormat Format>
struct AudioFormatTraits
{
    static constexpr bool sampleSigned = (SDL_AUDIO_MASK_SIGNED & Format) != 0;
    static constexpr bool sampleBigEndian = SDL_AUDIO_ISBIGENDIAN(Format);
    static constexpr bool sampleLittleEndian = SDL_AUDIO_ISLITTLEENDIAN(Format);
    static constexpr bool sampleFloating = SDL_AUDIO_ISFLOAT(Format);
    static constexpr bool sampleIntegral = SDL_AUDIO_ISINT(Format);
    static constexpr std::uint16_t sampleBitSize = SDL_AUDIO_BITSIZE(Format);

    using SampleType = detail::AudioSampleType<sampleFloating, sampleBitSize, sampleSigned>;
};

} // namespace sdlwrapper

#endif // SDLWRAPPER_AUDIO_HPP
