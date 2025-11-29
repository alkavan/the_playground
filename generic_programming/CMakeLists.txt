cmake_minimum_required(VERSION 3.28)
project(GenericProgramming CXX)

# Use 23 for compatibility; /std:c++latest enables C++26 previews like contracts
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_executable(generic_programming
        main.cpp
)

# Detect compiler and add experimental support for contracts
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
        target_compile_options(generic_programming PRIVATE -fcontracts -fcontract-continuation-mode=on)
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(generic_programming PRIVATE -fcontract-build-level=default)
            message(STATUS "GenericProgramming: contracts enabled with -fcontracts (default level)")
        else()
            target_compile_options(generic_programming PRIVATE -fcontract-build-level=audit)
            message(STATUS "GenericProgramming: contracts enabled with -fcontracts (audit level, extra asserts)")
        endif()
    else()
        message(WARNING "GenericProgramming: GCC/Clang <14 detected; contracts disabled")
    endif()

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC: No contracts support; use STL hardening for precondition checks
    target_compile_definitions(generic_programming PRIVATE _MSVC_STL_HARDENING=1)
    # Optional: Add /J7 for signed char literals if needed elsewhere
    target_compile_options(generic_programming PRIVATE /J7)
    message(STATUS "GenericProgramming: No contracts support; using STL hardening (_MSVC_STL_HARDENING=1)")

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(generic_programming PRIVATE /fcontract-build-level=default)
        message(STATUS "GenericProgramming: contracts enabled with /experimental:contracts (default level)")
    else()
        target_compile_options(generic_programming PRIVATE /fcontract-build-level=audit)
        message(STATUS "GenericProgramming: contracts enabled with /experimental:contracts (audit level, extra asserts)")
    endif()
else()
    message(WARNING "GenericProgramming: Unsupported compiler for contracts; disabled")
endif()
