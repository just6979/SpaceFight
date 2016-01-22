cmake_minimum_required(VERSION 3.3)
project("SpaceFight")

set(EXECUTABLE_NAME ${PROJECT_NAME})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

INCLUDE_DIRECTORIES(include)

INCLUDE_DIRECTORIES(contrib)
file(GLOB_RECURSE CONTRIB_SOURCE_C contrib *.c)
file(GLOB_RECURSE CONTRIB_SOURCE_CPP contrib *.cpp)

set(SOURCE_FILES
        include/Game.h
        include/Console.h
        include/GameSprite.h
        src/SpaceFight.cpp
        src/Game.cpp
        src/GameSprite.cpp
        src/Console.cpp
        src/GamePlayer.cpp include/GamePlayer.h)

if (WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
endif ()
find_package(SFML 2 REQUIRED graphics window system)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
endif ()

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${CONTRIB_SOURCE_C} ${CONTRIB_SOURCE_CPP})

target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
