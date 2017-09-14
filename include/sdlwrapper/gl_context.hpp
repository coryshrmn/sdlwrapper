#ifndef SDLWRAPPER_GL_CONTEXT_HPP
#define SDLWRAPPER_GL_CONTEXT_HPP

#include "sdlwrapper/window.hpp"

#include <cwrapper/resource.hpp>

#include <stdexcept>

namespace sdlwrapper
{

namespace detail
{
struct GLContextDeleter
{
    void operator()(SDL_GLContext handle)
    {
        SDL_GL_DeleteContext(handle);
    }
};
} // namespace detail

class GLContext
{
public:
    GLContext() = default;
    GLContext(const Window& window);

private:
    cwrapper::Resource<SDL_GLContext, detail::GLContextDeleter> _resource;
};

inline GLContext::GLContext(const Window &window)
    : _resource(SDL_GL_CreateContext(window.getHandle()))
{
    if(!_resource.hasHandle()) {
        throw std::runtime_error(SDL_GetError());
    }
}

} // namespace sdlwrapper

#endif // SDLWRAPPER_GL_CONTEXT_HPP
