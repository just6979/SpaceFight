cmake_minimum_required(VERSION 3.3)

project("JustAGame")

set(EXECUTABLE_NAME ${PROJECT_NAME})

## compiler options

# show all command lines in CLion
#set(CMAKE_VERBOSE_MAKEFILE ON)

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

## external libs

# logog
if (WIN32)
    # set LOGOG_ROOT in CLion CMake settings
    INCLUDE_DIRECTORIES(${LOGOG_ROOT}/include)
    LINK_DIRECTORIES(${LOGOG_ROOT}/lib)
endif()
set(EXTERNAL_LIBS logog ${EXTERNAL_LIBS})

## external libs with CMake find_package() support
## root dir for these is set in CLion CMake settings because it's system dependent

# yaml-cpp
set(CMAKE_PREFIX_PATH "${YAML_CPP_ROOT}/CMake" ${CMAKE_PREFIX_PATH})
find_package(yaml-cpp 0.5 REQUIRED CONFIG)
if (yaml-cpp_FOUND)
    include_directories(${YAML_CPP_INCLUDE_DIR})
endif()

# SFML
if (WIN32)
    # use static sfml libs on Windows
    set(SFML_STATIC_LIBRARIES TRUE)
endif ()
set(CMAKE_MODULE_PATH "${SFML_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED graphics window system)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
endif ()

## our code

# add local headers
INCLUDE_DIRECTORIES(include)
file(GLOB HEADER_FILES include/*.h)
# add all source files
file(GLOB SOURCE_FILES src/*.cpp)

## build it

# add output binary dependent on local headers & sources and contrib sources
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${CONTRIB_FILES})

# link with local objects and SFML libs & dependencies
target_link_libraries(${EXECUTABLE_NAME} ${EXTERNAL_LIBS} ${YAML_CPP_LIBRARIES} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
