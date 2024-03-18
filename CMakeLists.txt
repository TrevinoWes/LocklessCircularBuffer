cmake_minimum_required(VERSION 3.20)

project(LockLessCircBufferSample)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") 

add_subdirectory(CircularSPSCQueue)

if(MAKE_TESTS)
    add_subdirectory(Tests)
endif()