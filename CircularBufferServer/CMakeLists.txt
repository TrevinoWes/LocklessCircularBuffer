project(CircleBuffServer VERSION 1.0.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lstdc++fs -fsanitize=thread")

add_executable(${PROJECT_NAME} CircularBuffServer.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Shared-Types Socket-Factory CircularSPSCQueue Logger)
