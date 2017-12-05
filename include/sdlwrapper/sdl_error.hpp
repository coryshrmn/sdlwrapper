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
#ifndef SDLWRAPPER_SDL_ERROR_HPP
#define SDLWRAPPER_SDL_ERROR_HPP

#include "SDL2/SDL_error.h"

#include <stdexcept>

namespace sdlwrapper {

class SdlError : public std::runtime_error
{
public:
    SdlError()
        : std::runtime_error(SDL_GetError())
    { }
};

} // namespace sdlwrapper

#endif // SDLWRAPPER_SDL_ERROR_HPP
