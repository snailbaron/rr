add_library(mm
    mm.cpp
)
target_include_directories(mm PUBLIC include)
target_link_libraries(mm PRIVATE error)
