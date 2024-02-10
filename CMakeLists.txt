cmake_minimum_required(VERSION 3.24)

project(rr)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(X11 REQUIRED)
find_package(Vulkan)

add_compile_definitions(
    VK_NO_PROTOTYPES
    VK_USE_PLATFORM_XCB_KHR
)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_subdirectory(src)
add_subdirectory(example)
