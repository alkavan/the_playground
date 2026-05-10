cmake_minimum_required(VERSION 3.28)
project(Gnu26Lifetimes CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check for GCC 16.1 compiler or higher
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "This GNU C++26 example requires the GCC compiler."
                        "Detected compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "16.1")
    message(FATAL_ERROR
            "GCC 16.1 or higher is required for this C++26 example.\n"
            "Detected GCC version: ${CMAKE_CXX_COMPILER_VERSION}\n"
            "Please upgrade your GCC installation."
    )
endif()

add_executable(gnu26_lifetimes
        main.cpp
)

target_compile_options(gnu26_lifetimes PRIVATE
        -Wall
        -Wextra
        -Wno-missing-requires
)
