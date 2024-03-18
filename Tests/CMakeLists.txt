project(UnitTests VERSION 1.0.0)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME} CircularBuffer_UnitTest.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE CircularSPSCQueue gtest_main)