cmake_minimum_required(VERSION 3.20)

project(LockLessCircBufferSample)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -O3") 
SET(LOGGER_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Logger)

if(MAKE_PERF_TEST_BUILD)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fno-inline -g") 
endif()

add_subdirectory(Logger)
add_subdirectory(SharedTypes)
add_subdirectory(Socket_Factory)
add_subdirectory(CircularSPSCQueue)
add_subdirectory(Timer)

if(MAKE_TESTS)
    add_subdirectory(Tests)
endif()

add_subdirectory(CircularBufferServer)
add_subdirectory(CircularBufferClient)