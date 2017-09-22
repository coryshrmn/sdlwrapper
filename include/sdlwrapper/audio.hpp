#ifndef SDLWRAPPER_AUDIO_HPP
#define SDLWRAPPER_AUDIO_HPP

#include "sdlwrapper/sdl.hpp"
#include "sdlwrapper/detail/audio_sample_type.hpp"

#include <boost/integer.hpp>

#include <stdexcept>
#include <optional>
#include <functional>

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

namespace detail
{

struct WavDeleter
{
    void operator()(std::uint8_t* buf)
    {
        SDL_FreeWAV(buf);
    }
};

struct AudioDeviceDeleter
{
    void operator()(SDL_AudioDeviceID dev)
    {
        SDL_CloseAudioDevice(dev);
    }
};

} // namespace detail

class Wav
{
public:
    Wav() = default;
    explicit Wav(const AudioSubsystem&, const char* fileName);

    std::uint32_t getSizeBytes() const;

    int getFreq() const;

    AudioFormat getAudioFormat() const;

    std::uint8_t getChannels() const;

    std::uint8_t* begin();
    const std::uint8_t* begin() const;

    std::uint8_t* end();
    const std::uint8_t* end() const;

private:
    cwrapper::Resource<std::uint8_t*, detail::WavDeleter> _resource {};
    std::uint32_t _sizeBytes {};
    int _freq {};
    AudioFormat _format {};
    std::uint8_t _channels {};
};

class AudioDevice
{
public:
    using Callback = std::function<void(std::uint8_t* stream, int len)>;

    AudioDevice(const AudioSubsystem&, const char* name, bool capture, int freq, AudioFormat format, std::uint8_t channels, std::uint16_t samples, Callback callback, int allowedChanges = 0);
    AudioDevice(const AudioSubsystem&, const char* name, bool capture, int freq, AudioFormat format, std::uint8_t channels, std::uint16_t samples, SDL_AudioCallback callback, void* userData = nullptr, int allowedChanges = 0);

private:
    void init(const char* name, bool capture, const SDL_AudioSpec& desiredSpec, int allowedChanges);

    std::optional<Callback> _optCallback {};
    cwrapper::Resource<SDL_AudioDeviceID, detail::AudioDeviceDeleter> _resource {};
    SDL_AudioSpec _obtainedSpec {};

    static void dispatchCallback(void* userdata, std::uint8_t* stream, int len);
};

inline Wav::Wav(const AudioSubsystem&, const char *fileName)
{
    SDL_AudioSpec spec;
    std::uint8_t* buf;
    if(SDL_LoadWAV(fileName, &spec, &buf, &_sizeBytes) == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
    _resource.setHandle(buf);
    _freq = spec.freq;
    _format = spec.format;
    _channels = spec.channels;
}

uint32_t Wav::getSizeBytes() const
{
    return _sizeBytes;
}

int Wav::getFreq() const
{
    return _freq;
}

AudioFormat Wav::getAudioFormat() const
{
    return _format;
}

uint8_t Wav::getChannels() const
{
    return _channels;
}

uint8_t* Wav::begin()
{
    return _resource.getHandle();
}

const uint8_t* Wav::begin() const
{
    return _resource.getHandle();
}

uint8_t* Wav::end()
{
    return _resource.getHandle() + _sizeBytes;
}

const uint8_t* Wav::end() const
{
    return _resource.getHandle() + _sizeBytes;
}

AudioDevice::AudioDevice(const AudioSubsystem&, const char *name, bool capture, int freq, AudioFormat format, uint8_t channels, uint16_t samples, AudioDevice::Callback callback, int allowedChanges)
    : _optCallback(callback)
{
    SDL_AudioSpec desiredSpec {};
    desiredSpec.freq = freq;
    desiredSpec.format = format;
    desiredSpec.channels = channels;
    desiredSpec.samples = samples;
    desiredSpec.callback = dispatchCallback;
    desiredSpec.userdata = &_optCallback.value();

    init(name, capture, desiredSpec, allowedChanges);
}

AudioDevice::AudioDevice(const AudioSubsystem&, const char *name, bool capture, int freq, AudioFormat format, uint8_t channels, uint16_t samples, SDL_AudioCallback callback, void *userdata, int allowedChanges)
{
    SDL_AudioSpec desiredSpec {};
    desiredSpec.freq = freq;
    desiredSpec.format = format;
    desiredSpec.channels = channels;
    desiredSpec.samples = samples;
    desiredSpec.callback = callback;
    desiredSpec.userdata = userdata;

    init(name, capture, desiredSpec, allowedChanges);
}

void AudioDevice::init(const char *name, bool capture, const SDL_AudioSpec& desiredSpec, int allowedChanges)
{
    _resource.setHandle(SDL_OpenAudioDevice(name, capture, &desiredSpec, &_obtainedSpec, allowedChanges));
    if(!_resource.hasHandle()) {
        throw std::runtime_error(SDL_GetError());
    }
}

void AudioDevice::dispatchCallback(void *userdata, uint8_t *stream, int len)
{
    reinterpret_cast<Callback*>(userdata)->operator()(stream, len);
}


} // namespace sdlwrapper

#endif // SDLWRAPPER_AUDIO_HPP
