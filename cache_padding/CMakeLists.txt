
cmake_minimum_required(VERSION 3.28)
project(CachePadding CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

add_executable(cache_padding
        main.cpp
)

target_link_libraries(cache_padding Threads::Threads)
target_compile_options(cache_padding PRIVATE -O3 -march=native -pthread)
target_link_options(cache_padding PRIVATE -pthread)
