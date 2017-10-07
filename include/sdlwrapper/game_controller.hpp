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
#ifndef SDLWRAPPER_GAME_CONTROLLER_HPP
#define SDLWRAPPER_GAME_CONTROLLER_HPP

#include "sdlwrapper/sdl.hpp"

#include <cwrapper/resource.hpp>

#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>

namespace sdlwrapper
{

namespace detail
{

struct GameControllerDeleter
{
    void operator()(SDL_GameController* handle)
    {
        SDL_GameControllerClose(handle);
    }
};

} // namespace detail

class GameController
{
public:

    enum class Axis
    {
        INVALID = SDL_CONTROLLER_AXIS_INVALID,
        LEFTX = SDL_CONTROLLER_AXIS_LEFTX,
        LEFTY = SDL_CONTROLLER_AXIS_LEFTY,
        RIGHTX = SDL_CONTROLLER_AXIS_RIGHTX,
        RIGHTY = SDL_CONTROLLER_AXIS_RIGHTY,
        TRIGGERLEFT = SDL_CONTROLLER_AXIS_TRIGGERLEFT,
        TRIGGERRIGHT = SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
        MAX = SDL_CONTROLLER_AXIS_MAX
    };

    enum class Button
    {
        INVALID = SDL_CONTROLLER_BUTTON_INVALID,
        A = SDL_CONTROLLER_BUTTON_A,
        B = SDL_CONTROLLER_BUTTON_B,
        X = SDL_CONTROLLER_BUTTON_X,
        Y = SDL_CONTROLLER_BUTTON_Y,
        BACK = SDL_CONTROLLER_BUTTON_BACK,
        GUIDE = SDL_CONTROLLER_BUTTON_GUIDE,
        START = SDL_CONTROLLER_BUTTON_START,
        LEFTSTICK = SDL_CONTROLLER_BUTTON_LEFTSTICK,
        RIGHTSTICK = SDL_CONTROLLER_BUTTON_RIGHTSTICK,
        LEFTSHOULDER = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
        RIGHTSHOULDER = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
        DPAD_UP = SDL_CONTROLLER_BUTTON_DPAD_UP,
        DPAD_DOWN = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
        DPAD_LEFT = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
        DPAD_RIGHT = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
        MAX = SDL_CONTROLLER_BUTTON_MAX
    };

    static constexpr std::array<Button, 15> ALL_BUTTONS {
        Button::A,
        Button::B,
        Button::X,
        Button::Y,
        Button::BACK,
        Button::GUIDE,
        Button::START,
        Button::LEFTSTICK,
        Button::RIGHTSTICK,
        Button::LEFTSHOULDER,
        Button::RIGHTSHOULDER,
        Button::DPAD_UP,
        Button::DPAD_DOWN,
        Button::DPAD_LEFT,
        Button::DPAD_RIGHT
    };

    static constexpr std::array<Axis, 6> ALL_AXES {
        Axis::LEFTX,
        Axis::LEFTY,
        Axis::RIGHTX,
        Axis::RIGHTY,
        Axis::TRIGGERLEFT,
        Axis::TRIGGERRIGHT
    };

    GameController() = default;

    GameController(const GameControllerSubsystem& subsystem, int index);

    std::int16_t get(Axis axis) const;

    float getFloat(Axis axis) const;

    bool get(Button button) const;

    bool isAttached() const;

    SDL_JoystickID getInstanceID() const;

private:

    cwrapper::Resource<SDL_GameController*, detail::GameControllerDeleter> _resource {};
};

inline GameController::GameController(const GameControllerSubsystem&, int index)
    : _resource(SDL_GameControllerOpen(index))
{
    if(!_resource.hasHandle()) {
        throw std::runtime_error(SDL_GetError());
    }
}

inline int16_t GameController::get(GameController::Axis axis) const
{
    return SDL_GameControllerGetAxis(_resource.getHandle(), static_cast<SDL_GameControllerAxis>(axis));
}

inline float GameController::getFloat(GameController::Axis axis) const
{
    float val = static_cast<float>(get(axis)) / static_cast<float>(std::numeric_limits<int16_t>::max());
    // since original val is signed, it goes 1 further in negative direction than positive
    // so we clamp at -1.0f
    return std::max(-1.0f, val);
}

inline bool GameController::get(GameController::Button button) const
{
    return SDL_GameControllerGetButton(_resource.getHandle(), static_cast<SDL_GameControllerButton>(button));
}

inline bool GameController::isAttached() const
{
    return _resource.hasHandle() && SDL_GameControllerGetAttached(_resource.getHandle());
}

inline SDL_JoystickID GameController::getInstanceID() const
{
    SDL_Joystick* joystick = SDL_GameControllerGetJoystick(_resource.getHandle());
    if(joystick == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
    SDL_JoystickID id = SDL_JoystickInstanceID(joystick);
    if(id < 0) {
        throw std::runtime_error(SDL_GetError());
    }
    return id;
}

} // namespace sdlwrapper

#endif // SDLWRAPPER_GAME_CONTROLLER_HPP
