cmake_minimum_required(VERSION 4.1.1)

project(geimboi-app C CXX)

# Define the backend library
add_library(${PROJECT_NAME}
    entry_point.cpp
)
add_library(GeimBoi::app ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GEIMBOI_CORE_INCLUDE_DIRS}
)
