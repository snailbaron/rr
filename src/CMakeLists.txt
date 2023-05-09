add_library(rr
    rr.cpp
)
target_include_directories(rr PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rr PRIVATE
    X11::xcb
    Vulkan::Vulkan
)
