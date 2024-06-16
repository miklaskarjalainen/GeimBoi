cmake_minimum_required(VERSION 3.6)

project(imgui LANGUAGES CXX)

add_library(imgui
    ../../vendor/include/imgui/imgui.cpp 
    ../../vendor/include/imgui/imgui.h
    ../../vendor/include/imgui/imconfig.h
    ../../vendor/include/imgui/imgui_demo.cpp
    ../../vendor/include/imgui/imgui_draw.cpp
    ../../vendor/include/imgui/imgui_internal.h
    ../../vendor/include/imgui/imgui_tables.cpp
    ../../vendor/include/imgui/imgui_widgets.cpp
    ../../vendor/include/imgui/imgui.cpp
    ../../vendor/include/imgui/imstb_rectpack.h
    ../../vendor/include/imgui/imstb_truetype.h

    ../../vendor/include/imgui/backends/imgui_impl_sdl2.cpp
    ../../vendor/include/imgui/backends/imgui_impl_sdl2.h
    ../../vendor/include/imgui/backends/imgui_impl_opengl2.cpp
    ../../vendor/include/imgui/backends/imgui_impl_opengl2.h
)

if (WIN32)
    target_include_directories(imgui PUBLIC ../../vendor/include/imgui/ ../../vendor/include/WindowsOnly/SDL2)
else()
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
    find_package(SDL2 REQUIRED)

    include_directories(${SDL2_INCLUDE_DIRS})
    target_include_directories(imgui PUBLIC ../../vendor/include/imgui/)
endif()
