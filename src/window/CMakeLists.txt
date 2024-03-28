add_library(window
    window.cpp
)
target_include_directories(window PUBLIC include)
target_link_libraries(window PRIVATE error)

if(UNIX)
    target_sources(window PRIVATE xcb_window.cpp)
    target_link_libraries(window PRIVATE X11::xcb)
elseif(WIN32)
    target_sources(window PRIVATE windows_window.cpp)
endif()
