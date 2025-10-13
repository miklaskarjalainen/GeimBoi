cmake_minimum_required(VERSION 3.14.7)

# Add libs
add_subdirectory(src/core)
add_subdirectory(src/application)

project(
    GeimBoi C CXX
)
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE GeimBoi::core GeimBoi::app)
