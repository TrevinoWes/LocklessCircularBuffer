
add_library(
    Timer
    Timer.h
    Timer.cpp
)

target_link_libraries(Timer Shared-Types)
target_include_directories(Timer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")