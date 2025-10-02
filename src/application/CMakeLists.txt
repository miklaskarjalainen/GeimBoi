cmake_minimum_required(VERSION 4.1.1)

include(cmake/SDL3.cmake)
include(cmake/imgui.cmake)
include(cmake/imgui_club.cmake)

project(geimboi-app C CXX)

# Define the backend library
add_library(${PROJECT_NAME}
    App.cpp App.hpp
    entry_point.cpp
)
add_library(GeimBoi::app ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GEIMBOI_CORE_INCLUDE_DIRS}
    ${IMGUI_CLUB_SRC_DIR}/imgui_memory_editor/
)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE GeimBoi::core SDL3::SDL3-static imgui opengl32)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE GeimBoi::core SDL3::SDL3-static imgui GL)
endif()
