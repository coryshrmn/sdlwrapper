#include "gtest/gtest.h"

#include "sdlwrapper/audio.hpp"

#include <cstdint>
#include <cstring>
#include <type_traits>

using sdlwrapper::Sdl;
using sdlwrapper::SubsystemType;
using sdlwrapper::AudioFormat;
using sdlwrapper::AudioFormatTraits;
using sdlwrapper::Wav;
using sdlwrapper::AudioDevice;

TEST(SdlAudio, AudioFormatTraits) {
    EXPECT_TRUE(AudioFormatTraits<AUDIO_S8>::sampleSigned);
    EXPECT_FALSE(AudioFormatTraits<AUDIO_U8>::sampleSigned);
    EXPECT_TRUE(AudioFormatTraits<AUDIO_U8>::sampleIntegral);
    EXPECT_TRUE(AudioFormatTraits<AUDIO_S16>::sampleIntegral);
    EXPECT_FALSE(AudioFormatTraits<AUDIO_F32>::sampleIntegral);
    EXPECT_FALSE(AudioFormatTraits<AUDIO_S16>::sampleFloating);
    EXPECT_TRUE(AudioFormatTraits<AUDIO_F32>::sampleFloating);
    EXPECT_EQ(AudioFormatTraits<AUDIO_F32>::sampleBitSize, 32);
    EXPECT_EQ(AudioFormatTraits<AUDIO_S8>::sampleBitSize, 8);
    EXPECT_EQ(AudioFormatTraits<AUDIO_U8>::sampleBitSize, 8);
    EXPECT_EQ(AudioFormatTraits<AUDIO_S32>::sampleBitSize, 32);
    EXPECT_EQ(AudioFormatTraits<AUDIO_S16>::sampleBitSize, 16);

    EXPECT_TRUE((std::is_same_v<typename AudioFormatTraits<AUDIO_S8>::SampleType, std::int8_t>));
    EXPECT_TRUE((std::is_same_v<typename AudioFormatTraits<AUDIO_U8>::SampleType, std::uint8_t>));
    EXPECT_TRUE((std::is_same_v<typename AudioFormatTraits<AUDIO_S16>::SampleType, std::int16_t>));
    EXPECT_TRUE((std::is_same_v<typename AudioFormatTraits<AUDIO_U16>::SampleType, std::uint16_t>));
    EXPECT_TRUE((std::is_same_v<typename AudioFormatTraits<AUDIO_S32>::SampleType, std::int32_t>));
    EXPECT_TRUE((std::is_same_v<typename AudioFormatTraits<AUDIO_S32 ^ SDL_AUDIO_MASK_SIGNED>::SampleType, std::uint32_t>));
    EXPECT_TRUE((std::is_same_v<typename AudioFormatTraits<AUDIO_F32>::SampleType, std::float_t>));
    EXPECT_TRUE((std::is_same_v<typename AudioFormatTraits<AUDIO_F32 & ~SDL_AUDIO_MASK_BITSIZE | 64>::SampleType, std::double_t>));

    // SDL_AudioFormat only has 7 bits to represent bitsize
    // cannot represent 16 byte float samples
    // who the heck would do 16 byte floating audio anyway?
    if constexpr(sizeof(long double) < 16) {
        EXPECT_TRUE((std::is_same_v<typename AudioFormatTraits<AUDIO_F32 & ~SDL_AUDIO_MASK_BITSIZE | sizeof(long double) * 8>::SampleType, long double>));
    }
}

TEST(SdlAudio, LoadWavFloat) {
    Sdl<SubsystemType::AUDIO> sdl;

    Wav wav { sdl.audio(), SDLWRAPPER_TEST_RES_DIR "/para_open-01.wav" };

    EXPECT_EQ(wav.getAudioFormat(), AUDIO_F32);
    EXPECT_EQ(wav.getChannels(), 2);
    EXPECT_EQ(wav.getFreq(), 96000);

}

TEST(SdlAudio, AudioDevice) {
    Sdl<SubsystemType::AUDIO> sdl;

    auto callback = [](std::uint8_t* stream, int len) {
        std::memset(stream, 0, len);
    };

    AudioDevice device {sdl.audio(), nullptr, false, 96000, AUDIO_F32, 2, 4096, callback};

}
