cmake_minimum_required(VERSION 3.29)
project(CheckersMinimax)

include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
FETCHCONTENT_DECLARE(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.1)
FETCHCONTENT_MAKEAVAILABLE(SFML)

set(CMAKE_CXX_STANDARD 20)

add_executable(CheckersMinimax game.cpp
        game.h
        ScreenManager.h
        ScreenManager.cpp
        Checker.h
        Checker.cpp
        GameProps.h
)

target_link_libraries(CheckersMinimax sfml-graphics)


