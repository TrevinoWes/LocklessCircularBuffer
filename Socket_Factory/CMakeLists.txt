include(FetchContent)

add_library(
    Config-Parser
    ConfigParser.h
    ConfigParser.cpp
)

add_library(
    User-Input
    UserInput.h
    UserInput.cpp
)

add_library(
    Socket-Factory SHARED
    SocketFactory.h
    SocketFactory.cpp
)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(json)

target_link_libraries(Config-Parser PRIVATE Shared-Types nlohmann_json stdc++fs Logger)
target_link_libraries(Socket-Factory PRIVATE Shared-Types Config-Parser User-Input Logger)
target_include_directories(Socket-Factory PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")