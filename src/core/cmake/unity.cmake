# SPDX-License-Identifier: GPL-2.0-only

include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity
    GIT_TAG v2.6.1
)

FetchContent_MakeAvailable(unity)
