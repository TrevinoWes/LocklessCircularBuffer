add_library(
    CircularSPSCQueue
    CircularSPSCQueue.h
)

set_target_properties(CircularSPSCQueue PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(CircularSPSCQueue PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")