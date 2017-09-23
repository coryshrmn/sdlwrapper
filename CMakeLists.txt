# Copyright 2017 Cory Sherman
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# The sdlwrapper library is header only, and does not require any compilation.
# This CMakeLists.txt builds sdlwrapper tests.

# The sdlwrapper tests depend on:
# SDL2
# glew 2.0.0+

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(sdlwrapper-test)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(include)
include_directories("lib/cwrapper/include")

add_definitions("-DSDLWRAPPER_TEST_RES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/res\"")

set(SDLWRAPPER_HEADERS
    "include/sdlwrapper/audio.hpp"
    "include/sdlwrapper/detail/audio_sample_type.hpp"
    "include/sdlwrapper/gl_context.hpp"
    "include/sdlwrapper.hpp"
    "include/sdlwrapper/sdl.hpp"
    "include/sdlwrapper/window.hpp"
)

# main test
add_executable(sdlwrapper-test
    test/audio.cpp
    test/sdl.cpp
    ${SDLWRAPPER_HEADERS}
)
add_test(NAME sdlwrapper-test COMMAND sdlwrapper-test)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

# Download and compile Googletest library
include(ExternalProject)
set(gtest_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest-install")
ExternalProject_Add(gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.8.0
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${gtest_INSTALL_PREFIX}
)
set(gtest_INCLUDE_DIRS "${gtest_INSTALL_PREFIX}/include")
set(gtest_LIBRARIES
    "${gtest_INSTALL_PREFIX}/lib/libgtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${gtest_INSTALL_PREFIX}/lib/libgtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
add_dependencies(sdlwrapper-test gtest)
include_directories(${gtest_INCLUDE_DIRS})
target_link_libraries(sdlwrapper-test ${gtest_LIBRARIES})

# find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(sdlwrapper-test ${SDL2_LIBRARY})
