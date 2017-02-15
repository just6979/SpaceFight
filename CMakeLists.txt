CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

PROJECT("jage")

SET(EXECUTABLE_NAME ${PROJECT_NAME})

## compiler options

# show all command lines in CLion
set(CMAKE_VERBOSE_MAKEFILE ON)

# require compilers that do plain C11 & C++14
SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_STANDARD_REQUIRED ON)
SET(CMAKE_C_EXTENSIONS OFF)
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

# use debugging-aware optimization for debug builds
SET(DEBUG_FLAGS "-Og")

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS}")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")

# show all warnings for release builds
SET(RELEASE_FLAGS "${RELEASE_FLAGS} -Wall")
SET(RELEASE_FLAGS "${RELEASE_FLAGS} -Wextra")
SET(RELEASE_FLAGS "${RELEASE_FLAGS} -Werror")
#SET(RELEASE_FLAGS "${RELEASE_FLAGS} -pedantic")
#SET(RELEASE_FLAGS "${RELEASE_FLAGS} -pedantic-errors")

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${RELEASE_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS}")
SET(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${RELEASE_FLAGS}")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${RELEASE_FLAGS}")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${RELEASE_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${RELEASE_FLAGS}")

# use ccache if found
FIND_PROGRAM(CCACHE_FOUND ccache)
IF (CCACHE_FOUND)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
ENDIF (CCACHE_FOUND)

## local 3rd party libraries
INCLUDE_DIRECTORIES(contrib/easyloggingcpp/src)
FILE(GLOB CONTRIB_SOURCE_FILES contrib/easyloggingcpp/src/*.c*)

## external 3rd party libraries
# set LIB_ROOT to a unix-y file structure where the libs are:
# $LIB_ROOT/include, $LIB_ROOT/lib, etc,
# in CLion's CMake settings
IF (WIN32)
    # for Unixes, installing devel libs should put everything in the correct places
    # but for Windows, we'll use LIB_ROOT
    INCLUDE_DIRECTORIES(${LIB_ROOT}/include)
    SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${LIB_ROOT}/lib)
ENDIF ()

# logog
FIND_LIBRARY(LOGOG_LIBS logog)
SET(EXTERNAL_LIBS ${LOGOG_LIBS} ${EXTERNAL_LIBS})

## external 3rd party libraries with CMake find_package() support
## LIB_ROOT is set in CLion CMake settings because they're system dependent
## if a lib's CMake module isn't in LIB_ROOT, set WHAT_EVER_ROOT to its location

SET(CMAKE_PREFIX_PATH ${LIB_ROOT} ${CMAKE_PREFIX_PATH})
SET(CMAKE_MODULE_PATH "${LIB_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})

# yaml-cpp
# set YAML_CPP_ROOT in CLion/CMake if the CMake modules aren't with the libs
IF (NOT YAML_CPP_ROOT)
    SET(YAML_CPP_ROOT ${LIB_ROOT})
    SET(CMAKE_PREFIX_PATH ${YAML_CPP_ROOT} ${CMAKE_PREFIX_PATH})
ENDIF ()
FIND_PACKAGE(yaml-cpp 0.5 REQUIRED CONFIG)
IF (yaml-cpp_FOUND)
    INCLUDE_DIRECTORIES(${YAML_CPP_INCLUDE_DIR})
    SET(EXTERNAL_LIBS ${YAML_CPP_LIBRARIES} ${EXTERNAL_LIBS})
ENDIF ()

# SFML
# set SFML_ROOT in CLion/CMake if the CMake modules aren't with the libs
IF (NOT SFML_ROOT)
    SET(SFML_ROOT ${LIB_ROOT})
    SET(CMAKE_MODULE_PATH "${SFML_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})
ENDIF ()
# use static sfml libs
SET(SFML_STATIC_LIBRARIES TRUE)
FIND_PACKAGE(SFML 2 REQUIRED graphics window system)
IF (SFML_FOUND)
    INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
    SET(EXTERNAL_LIBS ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${EXTERNAL_LIBS})
ENDIF ()

## our code

# add local headers
INCLUDE_DIRECTORIES(include)
# add all source files
FILE(GLOB SOURCE_FILES src/*.cpp)

## build it

# add output binary dependent on local sources
ADD_EXECUTABLE(${EXECUTABLE_NAME} ${SOURCE_FILES} ${CONTRIB_SOURCE_FILES})

# link with local objects and SFML libs & dependencies
TARGET_LINK_LIBRARIES(
        ${EXECUTABLE_NAME}
        ${EXTERNAL_LIBS}
)
