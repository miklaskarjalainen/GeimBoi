# SPDX-License-Identifier: GPL-2.0-only

cmake_minimum_required(VERSION 3.14.7)

include(cmake/imgui.cmake)
include(cmake/imgui_club.cmake)
include(cmake/SDL3.cmake)
include(cmake/tomlplusplus.cmake)

project(geimboi-app C CXX)

# Define the backend library
add_library(${PROJECT_NAME}
    gui/GuiDebugger.cpp gui/GuiDebugger.hpp

    App.cpp App.hpp
    Settings.cpp Settings.hpp
    Window.cpp Window.hpp
    entry_point.cpp
)
add_library(GeimBoi::app ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GEIMBOI_CORE_INCLUDE_DIRS}
    ${IMGUI_CLUB_SRC_DIR}/imgui_memory_editor/
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE GeimBoi::core tomlplusplus::tomlplusplus SDL3::SDL3-static imgui opengl32)
else()
    # target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror)
    target_link_libraries(${PROJECT_NAME} PRIVATE GeimBoi::core tomlplusplus::tomlplusplus SDL3::SDL3-static imgui GL)
endif()
