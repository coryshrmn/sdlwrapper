# SDLWrapper

Header-only C++17 wrapper for SDL 2

## Status

Worked on as needed for my game. Mostly just RAII stuff for now.

## Using SDLWrapper

### Including SDLWrapper

You can add the [include](include) directory to your project,
and add `#include "sdlwrapper.hpp"` in your code.

### Compling SDLWrapper

SDLWrapper is header only, there is nothing to compile.

### Examples

See [test](test) files.

## Testing

SDLWrapper has a comprehensive unit test suite, using Googletest.
Tests are built with CMake.

### Testing Requirements

* CMake
* SDL2 development library

### Running the Tests

```bash
git clone https://github.com/coryshrmn/sdlwrapper.git
cd sdlwrapper
mkdir build && cd build
cmake ..
cmake --build .
./sdlwrapper-test
```
