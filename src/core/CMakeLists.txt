cmake_minimum_required(VERSION 4.1.1)

project(geimboi-core C)

set(GEIMBOI_CORE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# Define the backend library
add_library(${PROJECT_NAME} SHARED
    gbCore.h
    gbEmu.c gbEmu.h
    gbZ80.c gbZ80.h
    gbCart.c gbCart.h
    gbReg.h
)
add_library(GeimBoi::core ALIAS ${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_EXTENSIONS OFF)

# Specify include directories if needed
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler warnings
if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(WARNING "clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror)
    if (BS_TESTS)
        target_compile_options(tests PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror)
    endif()
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(WARNING "gnu")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror)
    if (BS_TESTS)
        target_compile_options(tests PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror)
    endif()
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    # @TODO
endif()
