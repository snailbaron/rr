add_executable(example
    main.cpp
)
target_link_libraries(example PRIVATE error window vk Vulkan::Headers)
