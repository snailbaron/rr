add_library(li
    li.cpp
)
target_include_directories(li PUBLIC include)
target_link_libraries(li PRIVATE error)