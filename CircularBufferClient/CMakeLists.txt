project(CircleBuffClient VERSION 1.0.0)

add_executable(${PROJECT_NAME} CircularBufferClient.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Shared-Types Socket-Factory Logger)