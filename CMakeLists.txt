cmake_minimum_required(VERSION 3.3)
project(SpaceFight)

set(EXECUTABLE_NAME "SpaceFight")

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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

set(SFML_ROOT "D:/lib/SFML-2.3.2/")
set(CMAKE_MODULE_PATH "${SFML_ROOT}/cmake/modules" ${CMAKE_MODULE_PATH})
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2 REQUIRED graphics window system)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
endif ()

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${LIBS})
