cmake_minimum_required(VERSION 3.3)
project("SpaceFight")

set(EXECUTABLE_NAME ${PROJECT_NAME})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(contrib/include)

set(SOURCE_FILES
        contrib/include/easylogging++.h
        include/Game.h
        include/GameSprite.h
        src/Game.cpp
        src/GameSprite.cpp
        src/SpaceFight.cpp
        )

if (WIN32)
    set(SFML_ROOT "D:/lib/SFML-2.3.2/")
    set(SFML_STATIC_LIBRARIES TRUE)
endif ()
if (UNIX)
    set(SFML_ROOT "/usr/share/SFML/")
endif ()
set(CMAKE_MODULE_PATH "${SFML_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED graphics window system)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
endif ()

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
