add_library(
    Shared-Types
    SharedTypes.h
)

set_target_properties(Shared-Types PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Shared-Types PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")