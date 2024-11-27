cmake_minimum_required(VERSION 3.29)
project(CheckersAttempt2)

include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
FETCHCONTENT_DECLARE(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.1)
FETCHCONTENT_MAKEAVAILABLE(SFML)

set(CMAKE_CXX_STANDARD 20)

add_executable(CheckersAttempt2 game.cpp
        game.h
        ScreenManager.h
        ScreenManager.cpp
        Constants.h
        Board.h
        Board.cpp
        WorldChecker.h
        WorldChecker.cpp)

target_link_libraries(CheckersAttempt2 sfml-graphics)