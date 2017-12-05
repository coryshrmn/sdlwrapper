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
#ifndef SDLWRAPPER_GL_CONTEXT_HPP
#define SDLWRAPPER_GL_CONTEXT_HPP

#include "sdlwrapper/sdl_error.hpp"
#include "sdlwrapper/window.hpp"

#include <cwrapper/resource.hpp>

#include <ostream>

namespace sdlwrapper
{

enum class GLAttribute
{
    RED_SIZE = SDL_GL_RED_SIZE,
    GREEN_SIZE = SDL_GL_GREEN_SIZE,
    BLUE_SIZE = SDL_GL_BLUE_SIZE,
    ALPHA_SIZE = SDL_GL_ALPHA_SIZE,
    BUFFER_SIZE = SDL_GL_BUFFER_SIZE,
    DOUBLEBUFFER = SDL_GL_DOUBLEBUFFER,
    DEPTH_SIZE = SDL_GL_DEPTH_SIZE,
    STENCIL_SIZE = SDL_GL_STENCIL_SIZE,
    ACCUM_RED_SIZE = SDL_GL_ACCUM_RED_SIZE,
    ACCUM_GREEN_SIZE = SDL_GL_ACCUM_GREEN_SIZE,
    ACCUM_BLUE_SIZE = SDL_GL_ACCUM_BLUE_SIZE,
    ACCUM_ALPHA_SIZE = SDL_GL_ACCUM_ALPHA_SIZE,
    STEREO = SDL_GL_STEREO,
    MULTISAMPLEBUFFERS = SDL_GL_MULTISAMPLEBUFFERS,
    MULTISAMPLESAMPLES = SDL_GL_MULTISAMPLESAMPLES,
    ACCELERATED_VISUAL = SDL_GL_ACCELERATED_VISUAL,
    CONTEXT_MAJOR_VERSION = SDL_GL_CONTEXT_MAJOR_VERSION,
    CONTEXT_MINOR_VERSION = SDL_GL_CONTEXT_MINOR_VERSION,
    CONTEXT_FLAGS = SDL_GL_CONTEXT_FLAGS,
    CONTEXT_PROFILE_MASK = SDL_GL_CONTEXT_PROFILE_MASK,
    SHARE_WITH_CURRENT_CONTEXT = SDL_GL_SHARE_WITH_CURRENT_CONTEXT,
    FRAMEBUFFER_SRGB_CAPABLE = SDL_GL_FRAMEBUFFER_SRGB_CAPABLE,
    CONTEXT_RELEASE_BEHAVIOR = SDL_GL_CONTEXT_RELEASE_BEHAVIOR
};

/**
 * @brief Call SDL_GL_SetAttribute() and throw on error.
 * @throws SdlError
 */
void setGLAttribute(GLAttribute attribute, int value);

/**
 * @brief Call SDL_GL_GetAttribute() and throw on error.
 * @throws SdlError
 */
int getGLAttribute(GLAttribute attribute);

const char* getGLAttributeName(GLAttribute attribute);

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

    enum class Profile
    {
        CORE = SDL_GL_CONTEXT_PROFILE_CORE,
        COMPATIBILITY = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
        ES = SDL_GL_CONTEXT_PROFILE_ES
    };

    GLContext() = default;
    GLContext(const Window& window);

    void makeCurrent(const Window& window);

    int getMajor() const { return _major; }
    int getMinor() const { return _minor; }
    Profile getProfile() const { return _profile; }

    static const char* getProfileName(Profile profile);

private:
    cwrapper::Resource<SDL_GLContext, detail::GLContextDeleter> _resource;

    int _major {};
    int _minor {};
    Profile _profile {};
};

inline void setGLAttribute(GLAttribute attribute, int value) {
    if(SDL_GL_SetAttribute(static_cast<SDL_GLattr>(attribute), value) != 0) {
        throw SdlError{};
    }
}

inline int getGLAttribute(GLAttribute attribute) {
    int value;
    if(SDL_GL_GetAttribute(static_cast<SDL_GLattr>(attribute), &value) != 0) {
        throw SdlError{};
    }
    return value;
}

inline const char* getGLAttributeName(GLAttribute attribute) {
    switch(attribute) {
    case GLAttribute::RED_SIZE: return "RED_SIZE";
    case GLAttribute::GREEN_SIZE: return "GREEN_SIZE";
    case GLAttribute::BLUE_SIZE: return "BLUE_SIZE";
    case GLAttribute::ALPHA_SIZE: return "ALPHA_SIZE";
    case GLAttribute::BUFFER_SIZE: return "BUFFER_SIZE";
    case GLAttribute::DOUBLEBUFFER: return "DOUBLEBUFFER";
    case GLAttribute::DEPTH_SIZE: return "DEPTH_SIZE";
    case GLAttribute::STENCIL_SIZE: return "STENCIL_SIZE";
    case GLAttribute::ACCUM_RED_SIZE: return "ACCUM_RED_SIZE";
    case GLAttribute::ACCUM_GREEN_SIZE: return "ACCUM_GREEN_SIZE";
    case GLAttribute::ACCUM_BLUE_SIZE: return "ACCUM_BLUE_SIZE";
    case GLAttribute::ACCUM_ALPHA_SIZE: return "ACCUM_ALPHA_SIZE";
    case GLAttribute::STEREO: return "STEREO";
    case GLAttribute::MULTISAMPLEBUFFERS: return "MULTISAMPLEBUFFERS";
    case GLAttribute::MULTISAMPLESAMPLES: return "MULTISAMPLESAMPLES";
    case GLAttribute::ACCELERATED_VISUAL: return "ACCELERATED_VISUAL";
    case GLAttribute::CONTEXT_MAJOR_VERSION: return "CONTEXT_MAJOR_VERSION";
    case GLAttribute::CONTEXT_MINOR_VERSION: return "CONTEXT_MINOR_VERSION";
    case GLAttribute::CONTEXT_FLAGS: return "CONTEXT_FLAGS";
    case GLAttribute::CONTEXT_PROFILE_MASK: return "CONTEXT_PROFILE_MASK";
    case GLAttribute::SHARE_WITH_CURRENT_CONTEXT: return "SHARE_WITH_CURRENT_CONTEXT";
    case GLAttribute::FRAMEBUFFER_SRGB_CAPABLE: return "FRAMEBUFFER_SRGB_CAPABLE";
    case GLAttribute::CONTEXT_RELEASE_BEHAVIOR: return "CONTEXT_RELEASE_BEHAVIOR";
    default: return "UNKNOWN";
    }
}


inline GLContext::GLContext(const Window &window)
    : _resource(SDL_GL_CreateContext(window.getHandle()))
    , _major(getGLAttribute(GLAttribute::CONTEXT_MAJOR_VERSION))
    , _minor(getGLAttribute(GLAttribute::CONTEXT_MINOR_VERSION))
    , _profile(static_cast<Profile>(getGLAttribute(GLAttribute::CONTEXT_PROFILE_MASK)))
{
    if(!_resource.hasHandle()) {
        throw SdlError{};
    }
}

inline void GLContext::makeCurrent(const Window &window)
{
    if(SDL_GL_MakeCurrent(window.getHandle(), _resource.getHandle()) != 0) {
        throw SdlError{};
    }
}

inline const char* GLContext::getProfileName(GLContext::Profile profile)
{
    switch(profile) {
    case Profile::CORE: return "CORE";
    case Profile::COMPATIBILITY: return "COMPATIBILITY";
    case Profile::ES: return "ES";
    default: return "UNKNOWN";
    }
}

} // namespace sdlwrapper

inline std::ostream& operator<<(std::ostream& out, const sdlwrapper::GLAttribute& attribute)
{
    return out << sdlwrapper::getGLAttributeName(attribute);
}

inline std::ostream& operator<<(std::ostream& out, const sdlwrapper::GLContext::Profile& profile)
{
    return out << sdlwrapper::GLContext::getProfileName(profile);
}

inline std::ostream& operator<<(std::ostream& out, const sdlwrapper::GLContext& context)
{
    return out << context.getMajor() << "." << context.getMinor() << " " << context.getProfile();
}

#endif // SDLWRAPPER_GL_CONTEXT_HPP
