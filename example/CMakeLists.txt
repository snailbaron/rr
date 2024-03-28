set(SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
configure_file(build-info.hpp.in include/build-info.hpp @ONLY)

add_custom_command(
    COMMENT "compile vertex shader"
    COMMAND Vulkan::glslc
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy.vert
        -o ${SHADER_DIR}/vert.spv
    DEPENDS dummy.vert
    OUTPUT ${SHADER_DIR}/vert.spv
)
add_custom_command(
    COMMENT "compile fragment shader"
    COMMAND Vulkan::glslc
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy.frag
        -o ${SHADER_DIR}/frag.spv
    DEPENDS dummy.frag
    OUTPUT ${SHADER_DIR}/frag.spv
)

add_executable(example
    main.cpp
    ${SHADER_DIR}/vert.spv
    ${SHADER_DIR}/frag.spv
)
target_include_directories(example PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(example PRIVATE
    error li mm window Vulkan::Headers)
