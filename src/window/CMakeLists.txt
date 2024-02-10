add_library(window
    window.cpp
    xcb_window.cpp
)
target_include_directories(window PUBLIC include)
target_link_libraries(window PRIVATE error X11::xcb)
