cmake_minimum_required(VERSION 3.24)

project(rr)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(X11 REQUIRED)
find_package(Vulkan REQUIRED)

add_compile_definitions(
    VK_NO_PROTOTYPES
)

if(WIN32)
    add_compile_definitions(
        VK_USE_PLATFORM_WIN32_KHR
    )
elseif(UNIX)
    add_compile_definitions(
        VK_USE_PLATFORM_XCB_KHR
    )
endif()

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_compile_definitions(
    VULKAN_HPP_NO_CONSTRUCTORS
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)
add_subdirectory(src)
add_subdirectory(example)
