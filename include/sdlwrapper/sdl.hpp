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
#ifndef SDLWRAPPER_SDL_HPP
#define SDLWRAPPER_SDL_HPP

#include "sdlwrapper/sdl_error.hpp"

#include <cwrapper/enum.hpp>
#include <cwrapper/resource.hpp>

#include <SDL.h>

#include <cassert>
#include <cstdint>
#include <ostream>
#include <type_traits>

namespace sdlwrapper
{

enum class SubsystemType : std::uint32_t
{
    TIMER = SDL_INIT_TIMER,
    AUDIO = SDL_INIT_AUDIO,
    VIDEO = SDL_INIT_VIDEO,
    JOYSTICK = SDL_INIT_JOYSTICK,
    HAPTIC = SDL_INIT_HAPTIC,
    GAMECONTROLLER = SDL_INIT_GAMECONTROLLER,
    EVENTS = SDL_INIT_EVENTS,
    EVERYTHING = SDL_INIT_EVERYTHING
};

} // namespace sdlwrapper

namespace cwrapper
{
template <>
struct EnumTraits<sdlwrapper::SubsystemType>
{
    constexpr static bool isBitFlag = true;
};
} // namespace cwrapper

namespace sdlwrapper
{

// Subsystems don't actually do anything
// but having one proves the subsystem is initialized
template <SubsystemType>
struct Subsystem {};

using TimerSubsystem = Subsystem<SubsystemType::TIMER>;
using AudioSubsystem = Subsystem<SubsystemType::AUDIO>;
using VideoSubsystem = Subsystem<SubsystemType::VIDEO>;
using JoystickSubsystem = Subsystem<SubsystemType::JOYSTICK>;
using HapticSubsystem = Subsystem<SubsystemType::HAPTIC>;
using GameControllerSubsystem = Subsystem<SubsystemType::GAMECONTROLLER>;
using EventsSubsystem = Subsystem<SubsystemType::EVENTS>;

namespace detail
{
struct SdlDeleter
{
    void operator()(bool)
    {
        SDL_Quit();
    }
};
} // namespace detail

template <SubsystemType Flags>
class Sdl
{
private:
    template <SubsystemType S>
    Subsystem<S> getSubsystem() const;

    cwrapper::Resource<bool, detail::SdlDeleter> _resource;

public:

    Sdl();

    template <SubsystemType F = Flags, typename = std::enable_if_t<(F & SubsystemType::TIMER) == SubsystemType::TIMER>>
    Subsystem<SubsystemType::TIMER> timer() const { return getSubsystem<SubsystemType::TIMER>(); }

    template <SubsystemType F = Flags, typename = std::enable_if_t<(F & SubsystemType::AUDIO) == SubsystemType::AUDIO>>
    Subsystem<SubsystemType::AUDIO> audio() const { return getSubsystem<SubsystemType::AUDIO>(); }

    template <SubsystemType F = Flags, typename = std::enable_if_t<(F & SubsystemType::VIDEO) == SubsystemType::VIDEO>>
    Subsystem<SubsystemType::VIDEO> video() const { return getSubsystem<SubsystemType::VIDEO>(); }

    template <SubsystemType F = Flags, typename = std::enable_if_t<(F & (SubsystemType::JOYSTICK | SubsystemType::GAMECONTROLLER)) != static_cast<SubsystemType>(0)>>
    Subsystem<SubsystemType::JOYSTICK> joystick() const { return getSubsystem<SubsystemType::JOYSTICK>(); }

    template <SubsystemType F = Flags, typename = std::enable_if_t<(F & SubsystemType::HAPTIC) == SubsystemType::HAPTIC>>
    Subsystem<SubsystemType::HAPTIC> haptic() const { return getSubsystem<SubsystemType::HAPTIC>(); }

    template <SubsystemType F = Flags, typename = std::enable_if_t<(F & SubsystemType::GAMECONTROLLER) == SubsystemType::GAMECONTROLLER>>
    Subsystem<SubsystemType::GAMECONTROLLER> gamecontroller() const { return getSubsystem<SubsystemType::GAMECONTROLLER>(); }

    template <SubsystemType F = Flags, typename = std::enable_if_t<(F & (SubsystemType::EVENTS | SubsystemType::VIDEO | SubsystemType::JOYSTICK | SubsystemType::GAMECONTROLLER)) != static_cast<SubsystemType>(0)>>
    Subsystem<SubsystemType::EVENTS> events() const { return getSubsystem<SubsystemType::EVENTS>(); }
};

template <SubsystemType Flags>
Sdl<Flags>::Sdl()
    : _resource(true)
{
    if(SDL_Init(static_cast<std::uint32_t>(Flags)) != 0) {
        throw SdlError{};
    }
}

template <SubsystemType Flags>
template <SubsystemType S>
Subsystem<S> Sdl<Flags>::getSubsystem() const
{
    assert(_resource.hasHandle());
    return {};
}

} // namespace sdlwrapper

inline std::ostream& operator<<(std::ostream& out, const SDL_version& sdlVersion)
{
    return out << static_cast<int>(sdlVersion.major) << "." << static_cast<int>(sdlVersion.minor) << "." << static_cast<int>(sdlVersion.patch);
}

#endif // SDLWRAPPER_SDL_HPP
