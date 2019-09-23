cmake_minimum_required(VERSION 2.8.8)
project(ECE563_Project1A)

add_library(src "Project Code/cache.cpp" "Project Code/cache.h" "Project Code/main.cpp")
add_subdirectory(Benchmarks)

add_executable(main "Project Code/main.cpp")
target_link_libraries(main src)