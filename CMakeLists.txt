cmake_minimum_required(VERSION 3.24)

project(rr)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(X11 REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(src)
add_subdirectory(examples)
