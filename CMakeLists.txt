cmake_minimum_required(VERSION 3.6)
project(
    GeimBoi 
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES
    src/main.cpp
    src/version.hpp

    src/application/appGui.cpp src/application/appGui.hpp
    src/application/appSettings.cpp src/application/appSettings.hpp
    src/application/appWindow.cpp src/application/appWindow.hpp
    src/application/gui/rebindButton.cpp src/application/gui/rebindButton.hpp

    src/core/apu/apuChannel1.cpp src/core/apu/apuChannel1.hpp
    src/core/apu/apuChannel2.cpp src/core/apu/apuChannel2.hpp
    src/core/apu/apuChannel3.cpp src/core/apu/apuChannel3.hpp
    src/core/apu/apuSquare.cpp src/core/apu/apuSquare.hpp
    src/core/apu/soundWaves.cpp src/core/apu/soundWaves.hpp

    src/core/mbc/gbMBC.cpp src/core/mbc/gbMBC.hpp
    src/core/mbc/gbMBC1.cpp src/core/mbc/gbMBC1.hpp
    src/core/mbc/gbMBC2.cpp src/core/mbc/gbMBC2.hpp
    src/core/mbc/gbMBC3.cpp src/core/mbc/gbMBC3.hpp

    src/core/opcode/cb_opcode.cpp src/core/opcode/opcode.cpp
    src/core/opcode/debugger.cpp src/core/opcode/debugger.hpp

    src/core/gbAPU.cpp     src/core/gbAPU.hpp
    src/core/gbBootRom.cpp src/core/gbBootRom.hpp
    src/core/gbCart.cpp    src/core/gbCart.hpp
    src/core/gbGameBoy.cpp src/core/gbGameBoy.hpp
    src/core/gbPPU.cpp     src/core/gbPPU.hpp
    src/core/gbZ80.cpp     src/core/gbZ80.hpp
    src/core/reg.hpp
)
set(INCLUDE_DIRS
    vendor/include
    vendor/include/imgui/backends
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_subdirectory(cmake/imgui)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PRIVATE GeimBoi.rc)
endif()

if (WIN32)
    list(APPEND INCLUDE_DIRS vendor/include/WindowsOnly)
    target_link_directories(${PROJECT_NAME} PRIVATE vendor/lib)
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        SDL2.lib
        SDL2main.lib
        SDL2test.lib
        opengl32.lib
        imgui
    )
else()
    target_link_directories(${PROJECT_NAME} PRIVATE vendor/include/imgui)
    target_link_libraries(${PROJECT_NAME} imgui)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
