cmake_minimum_required(VERSION 3.3)

project("SpaceFight")

set(EXECUTABLE_NAME ${PROJECT_NAME})

## compiler options

# show all command lines in CLion
set(CMAKE_VERBOSE_MAKEFILE ON)

# require compilers that do C11 & C++14
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# don't use any non-standard compiler extensions
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# use debugging-aware optimization for debug builds
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

# show all warnings for release builds
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

# use ccache if found
find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

## extra options

# local #define to hide debugging code for release builds
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-DDO_DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

## local libraries that are compiled directly

# we have none currently

# get all local library files together
#set(CONTRIB_FILES)

## local libraries with CMake support

add_subdirectory(contrib/logog)
INCLUDE_DIRECTORIES(contrib/logog/include)

add_subdirectory(contrib/yaml-cpp)
INCLUDE_DIRECTORIES(contrib/yaml-cpp/include)

# but turn off testing
set(BUILD_TESTING off)

## external libs with CMake support

# set static sfml libs on Windows (less DLL hell)
if (WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
endif ()
# find SFML (SFML_ROOT is the SFML install directory, or the usr/share directory)
set(CMAKE_MODULE_PATH "${SFML_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED graphics window system)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
endif ()

## my code

# add local headers
INCLUDE_DIRECTORIES(include)
file(GLOB HEADER_FILES include/*.h)
# add all source files
file(GLOB SOURCE_FILES src/*.cpp)

## build it

# add output binary dependent on local headers & sources and contrib sources
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${CONTRIB_FILES})

# link with local objects and SFML libs & dependencies
target_link_libraries(${EXECUTABLE_NAME} logog yaml-cpp ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
