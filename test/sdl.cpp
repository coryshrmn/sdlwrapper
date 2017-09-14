#include "gtest/gtest.h"

#include "sdlwrapper/sdl.hpp"

using sdlwrapper::Sdl;
using sdlwrapper::SubsystemType;

TEST(Sdl, InitAll) {
    Sdl<SubsystemType::EVERYTHING> context;
}
