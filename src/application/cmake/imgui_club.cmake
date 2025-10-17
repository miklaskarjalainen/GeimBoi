# SPDX-License-Identifier: GPL-2.0-only

include(FetchContent)

FetchContent_Declare(
    imgui_club
    GIT_REPOSITORY https://github.com/ocornut/imgui_club
    GIT_TAG origin/main
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(imgui_club)
FetchContent_GetProperties(imgui_club SOURCE_DIR IMGUI_CLUB_SRC_DIR)
