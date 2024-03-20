cmake_minimum_required(VERSION 3.20)

project(LockLessCircBufferSample)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC") 
SET(LOGGER_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Logger)

add_subdirectory(Logger)
add_subdirectory(SharedTypes)
add_subdirectory(Socket_Factory)
add_subdirectory(CircularSPSCQueue)

if(MAKE_TESTS)
    add_subdirectory(Tests)
endif()
add_subdirectory(CircularBufferServer)
#add_subdirectory(CircularBufferClient)