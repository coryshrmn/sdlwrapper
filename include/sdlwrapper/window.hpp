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
#ifndef SDLWRAPPER_WINDOW_HPP
#define SDLWRAPPER_WINDOW_HPP

#include "sdlwrapper/sdl.hpp"
#include <stdexcept>

namespace sdlwrapper
{

enum class WindowFlags : std::uint32_t
{
    FULLSCREEN = SDL_WINDOW_FULLSCREEN,
    FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP,
    OPENGL = SDL_WINDOW_OPENGL,
    SHOWN = SDL_WINDOW_SHOWN,
    HIDDEN = SDL_WINDOW_HIDDEN,
    BORDERLESS = SDL_WINDOW_BORDERLESS,
    RESIZABLE = SDL_WINDOW_RESIZABLE,
    MINIMIZED = SDL_WINDOW_MINIMIZED,
    MAXIMIZED = SDL_WINDOW_MAXIMIZED,
    INPUT_GRABBED = SDL_WINDOW_INPUT_GRABBED,
    INPUT_FOCUS = SDL_WINDOW_INPUT_FOCUS,
    MOUSE_FOCUS = SDL_WINDOW_MOUSE_FOCUS,
    FOREIGN = SDL_WINDOW_FOREIGN,
    ALLOW_HIGHDPI = SDL_WINDOW_ALLOW_HIGHDPI,
    MOUSE_CAPTURE = SDL_WINDOW_MOUSE_CAPTURE,
    ALWAYS_ON_TOP = SDL_WINDOW_ALWAYS_ON_TOP,
    SKIP_TASKBAR = SDL_WINDOW_SKIP_TASKBAR,
    UTILITY = SDL_WINDOW_UTILITY,
    TOOLTIP = SDL_WINDOW_TOOLTIP,
    POPUP_MENU = SDL_WINDOW_POPUP_MENU
};

} // namespace sdlwrapper

namespace cwrapper
{
template <>
struct EnumTraits<sdlwrapper::WindowFlags>
{
    constexpr static bool isBitFlag = true;
};
} // namespace cwrapper

namespace sdlwrapper
{

namespace detail
{

struct WindowDeleter
{
    void operator()(SDL_Window* handle)
    {
        SDL_DestroyWindow(handle);
    };
};

} // namespace detail

class Window
{
public:
    Window() = default;
    Window(const VideoSubsystem&, const char* title, int x, int y, int w, int h, WindowFlags windowFlags);

    bool hasHandle() const;
    SDL_Window* getHandle() const;

private:
    cwrapper::Resource<SDL_Window*, detail::WindowDeleter> _resource;
};

inline Window::Window(const VideoSubsystem &, const char *title, int x, int y, int w, int h, WindowFlags flags)
    : _resource(SDL_CreateWindow(title, x, y, w, h, static_cast<std::uint32_t>(flags)))
{
    if(!_resource.hasHandle()) {
        throw std::runtime_error(SDL_GetError());
    }
}

inline bool Window::hasHandle() const
{
    return _resource.hasHandle();
}

inline SDL_Window *Window::getHandle() const
{
    return _resource.getHandle();
}

} // namespace sdlwrapper

#endif // SDLWRAPPER_WINDOW_HPP
