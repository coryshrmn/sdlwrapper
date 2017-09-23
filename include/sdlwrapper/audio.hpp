/*
   Copyright 2017 Cory Sherman

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
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

enum class AudioSpecChanges : int
{
    FREQUENCY = SDL_AUDIO_ALLOW_FREQUENCY_CHANGE,
    FORMAT = SDL_AUDIO_ALLOW_FORMAT_CHANGE,
    CHANNELS = SDL_AUDIO_ALLOW_CHANNELS_CHANGE,
    ANY = SDL_AUDIO_ALLOW_ANY_CHANGE
};

class AudioDevice
{
public:
    using Callback = std::function<void(std::uint8_t* stream, int len)>;

    /**
     * @brief Open an audio device with a std::function callback.
     *
     * @param name  Device name
     * @param capture  Open a recording device instead of an output device
     * @param freq  Sample rate in Hz
     * @param format  Format for each sample
     * @param channels  Number of audio channels
     * @param samples  Buffer size in samples
     * @param callback  Function to call when audio is required (for output devices) or available (for capture devices)
     * @param allowedChanges  Any of the AudioSpecChanges bit flags OR'd together.
     */
    AudioDevice(const AudioSubsystem&, const char* name, bool capture, int freq, AudioFormat format, std::uint8_t channels, std::uint16_t samples, Callback callback, AudioSpecChanges allowedChanges = {});

    /**
     * @brief Open an audio device with raw SDL_AudioCallback and UserData.
     *
     * @param name  Device name
     * @param capture  Open a recording device instead of an output device
     * @param freq  Sample rate in Hz
     * @param format  Format for each sample
     * @param channels  Number of audio channels
     * @param samples  Buffer size in samples
     * @param callback  Function to call when audio is required (for output devices) or available (for capture devices)
     * @param userData  Passed to callback.
     * @param allowedChanges  Any of the AudioSpecChanges bit flags OR'd together.
     */
    AudioDevice(const AudioSubsystem&, const char* name, bool capture, int freq, AudioFormat format, std::uint8_t channels, std::uint16_t samples, SDL_AudioCallback callback, void* userData = nullptr, AudioSpecChanges allowedChanges = {});

    /**
     * @brief Open an audio device with no callback, to be used with queue or deque.
     *
     * @param name  Device name
     * @param capture  Open a recording device instead of an output device
     * @param freq  Sample rate in Hz
     * @param format  Format for each sample
     * @param channels  Number of audio channels
     * @param samples  Buffer size in samples
     * @param allowedChanges  Any of the AudioSpecChanges bit flags OR'd together.
     */
    AudioDevice(const AudioSubsystem&, const char* name, bool capture, int freq, AudioFormat format, std::uint8_t channels, std::uint16_t samples, AudioSpecChanges allowedChanges = {});

    SDL_AudioStatus getStatus() const;

    const SDL_AudioSpec& getObtainedSpec() const;

    void play();
    void pause();

    void lock();
    void unlock();

    /**
     * @brief Queue audio for a non-callback output device.
     * @param data  Source buffer
     * @param len  Buffer length in bytes
     */
    void queue(const void* data, std::uint32_t len);

    /**
     * @brief Dequeue recorded audio from a non-callback capture device.
     * @param data  Destination buffer
     * @param len  Buffer length in bytes
     * @return Number of bytes stored in destination buffer
     */
    std::uint32_t dequeue(void* data, std::uint32_t len);

    /**
     * @brief Clear the queue of a non-callback output device.
     */
    void clearQueue();

    /**
     * @brief Get the number of bytes queued for playback of a non-callback output device.
     * @return Number of bytes queued but not yet sent to hardware.
     */
    std::uint32_t getQueueSize() const;

    static int getNumAudioDevices(const AudioSubsystem&, bool capture = false);
    static const char* getAudioDeviceName(const AudioSubsystem&, int index, bool capture = false);

private:
    void init(const char* name, bool capture, const SDL_AudioSpec& desiredSpec, AudioSpecChanges allowedChanges);

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

inline uint32_t Wav::getSizeBytes() const
{
    return _sizeBytes;
}

inline int Wav::getFreq() const
{
    return _freq;
}

inline AudioFormat Wav::getAudioFormat() const
{
    return _format;
}

inline uint8_t Wav::getChannels() const
{
    return _channels;
}

inline uint8_t* Wav::begin()
{
    return _resource.getHandle();
}

inline const uint8_t* Wav::begin() const
{
    return _resource.getHandle();
}

inline uint8_t* Wav::end()
{
    return _resource.getHandle() + _sizeBytes;
}

inline const uint8_t* Wav::end() const
{
    return _resource.getHandle() + _sizeBytes;
}

inline AudioDevice::AudioDevice(const AudioSubsystem&, const char *name, bool capture, int freq, AudioFormat format, uint8_t channels, uint16_t samples, AudioDevice::Callback callback, AudioSpecChanges allowedChanges)
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

inline AudioDevice::AudioDevice(const AudioSubsystem&, const char *name, bool capture, int freq, AudioFormat format, uint8_t channels, uint16_t samples, SDL_AudioCallback callback, void *userdata, AudioSpecChanges allowedChanges)
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

inline AudioDevice::AudioDevice(const AudioSubsystem &, const char *name, bool capture, int freq, AudioFormat format, uint8_t channels, uint16_t samples, AudioSpecChanges allowedChanges)
{
    SDL_AudioSpec desiredSpec {};
    desiredSpec.freq = freq;
    desiredSpec.format = format;
    desiredSpec.channels = channels;
    desiredSpec.samples = samples;

    init(name, capture, desiredSpec, allowedChanges);
}

inline SDL_AudioStatus AudioDevice::getStatus() const
{
    assert(_resource.hasHandle());
    return SDL_GetAudioDeviceStatus(_resource.getHandle());
}

inline const SDL_AudioSpec& AudioDevice::getObtainedSpec() const
{
    assert(_resource.hasHandle());
    return _obtainedSpec;
}

inline void AudioDevice::play()
{
    assert(_resource.hasHandle());
    SDL_PauseAudioDevice(_resource.getHandle(), false);
}

inline void AudioDevice::pause()
{
    assert(_resource.hasHandle());
    SDL_PauseAudioDevice(_resource.getHandle(), true);
}

inline void AudioDevice::lock()
{
    assert(_resource.hasHandle());
    SDL_LockAudioDevice(_resource.getHandle());
}

inline void AudioDevice::unlock()
{
    assert(_resource.hasHandle());
    SDL_UnlockAudioDevice(_resource.getHandle());
}

inline void AudioDevice::queue(const void* data, uint32_t len)
{
    assert(_resource.hasHandle());
    SDL_QueueAudio(_resource.getHandle(), data, len);
}

inline uint32_t AudioDevice::dequeue(void* data, uint32_t len)
{
    assert(_resource.hasHandle());
    return SDL_DequeueAudio(_resource.getHandle(), data, len);
}

inline void AudioDevice::clearQueue()
{
    assert(_resource.hasHandle());
    SDL_ClearQueuedAudio(_resource.getHandle());
}

inline uint32_t AudioDevice::getQueueSize() const
{
    assert(_resource.hasHandle());
    return SDL_GetQueuedAudioSize(_resource.getHandle());
}

inline int AudioDevice::getNumAudioDevices(const AudioSubsystem&, bool capture)
{
    return SDL_GetNumAudioDevices(capture);
}

inline const char* AudioDevice::getAudioDeviceName(const AudioSubsystem&, int index, bool capture)
{
    const char* name = SDL_GetAudioDeviceName(index, capture);
    if(name == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
    return name;
}

inline void AudioDevice::init(const char *name, bool capture, const SDL_AudioSpec& desiredSpec, AudioSpecChanges allowedChanges)
{
    _resource.setHandle(SDL_OpenAudioDevice(name, capture, &desiredSpec, &_obtainedSpec, static_cast<int>(allowedChanges)));
    if(!_resource.hasHandle()) {
        throw std::runtime_error(SDL_GetError());
    }
}

inline void AudioDevice::dispatchCallback(void *userdata, uint8_t *stream, int len)
{
    reinterpret_cast<Callback*>(userdata)->operator()(stream, len);
}

} // namespace sdlwrapper

namespace cwrapper
{

template <>
struct EnumTraits<sdlwrapper::AudioSpecChanges>
{
    constexpr static bool isBitFlag = false;
};

} // namespace cwrapper

#endif // SDLWRAPPER_AUDIO_HPP
