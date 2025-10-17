# SPDX-License-Identifier: GPL-2.0-only

set(BUILD_SHARED_LIBS OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG origin/main
)

FetchContent_MakeAvailable(SDL3)
FetchContent_GetProperties(SDL3 SOURCE_DIR SDL3_INCLUDE_DIRS)

set(SDL3_INCLUDE_DIRS ${SDL3_INCLUDE_DIRS}/include)
