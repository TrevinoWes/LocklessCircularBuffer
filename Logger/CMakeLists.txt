add_library(
    Logger SHARED
    Logger.h
)

set_target_properties(Logger PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Logger PRIVATE stdc++fs)
target_include_directories(Logger PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")