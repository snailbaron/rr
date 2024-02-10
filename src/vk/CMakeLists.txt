add_library(vk
    check.cpp
    device.cpp
    global.cpp
    instance.cpp
)
target_include_directories(vk PUBLIC include)
target_link_libraries(vk PRIVATE error Vulkan::Headers)
