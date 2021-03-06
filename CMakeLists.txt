CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

PROJECT("jage")

SET(EXECUTABLE_NAME ${PROJECT_NAME})

## compiler options

# show all command lines in CLion
SET(CMAKE_VERBOSE_MAKEFILE ON)

# require compilers that do plain C11 & C++14
SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_STANDARD_REQUIRED ON)
SET(CMAKE_C_EXTENSIONS OFF)
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

IF (MSVC)
	# use standard C++14 (as much as possible) even in MSVC
	SET(COMMON_FLAGS ${COMMON_FLAGS} " /std:c++14")
	# doesn't seem to work for VS2015 generator, should add manually to CMAKE_CXX_FLAGS during CMake configure
ELSE ()
	# warn about any implicit conversions
	SET(COMMON_FLAGS ${COMMON_FLAGS} "-Wconversion")

	# use debugging-aware optimization for debug builds
	SET(DEBUG_FLAGS "-Og")

	# show lots more warnings for release builds
	SET(RELEASE_FLAGS "${RELEASE_FLAGS} -Wall")
	SET(RELEASE_FLAGS "${RELEASE_FLAGS} -Wextra")
	# skip these warnings for easylogging++, lots of unused vars currently
	SET(RELEASE_FLAGS "${RELEASE_FLAGS} -Wno-unused-variable")
	# can't use this yet because elpp also has some conversion errors
	#SET(RELEASE_FLAGS "${RELEASE_FLAGS} -Werror")
	SET(RELEASE_FLAGS "${RELEASE_FLAGS} -pedantic")
	SET(RELEASE_FLAGS "${RELEASE_FLAGS} -pedantic-errors")
ENDIF ()

SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${COMMON_FLAGS})
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${COMMON_FLAGS})

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS}")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")

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
	# for Unixes, installing -dev libraries should put everything in the correct places
	# but for Windows, we'll use LIB_ROOT
	INCLUDE_DIRECTORIES(${LIB_ROOT}/include)
	SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${LIB_ROOT}/lib)
ENDIF ()

## external 3rd party libraries with CMake find_package() support

# LIB_ROOT is set in CLion CMake settings because it's system dependent
# if a lib's CMake module isn't in LIB_ROOT, set WHAT_EVER_ROOT to its location
SET(CMAKE_PREFIX_PATH ${LIB_ROOT} ${CMAKE_PREFIX_PATH})
SET(CMAKE_MODULE_PATH "${LIB_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})

# yaml-cpp
# set YAML_CPP_ROOT in CLion/CMake if the CMake modules aren't with the libs
IF (NOT YAML_CPP_ROOT)
	SET(YAML_CPP_ROOT ${LIB_ROOT})
ENDIF ()
SET(CMAKE_PREFIX_PATH ${YAML_CPP_ROOT} ${CMAKE_PREFIX_PATH})
FIND_PACKAGE(yaml-cpp 0.5 REQUIRED CONFIG)
IF (yaml-cpp_FOUND)
	INCLUDE_DIRECTORIES(${YAML_CPP_INCLUDE_DIR})
	SET(EXTERNAL_LIBS ${YAML_CPP_LIBRARIES} ${EXTERNAL_LIBS})
ENDIF ()

# SFML
# set SFML_ROOT in CLion/CMake if the CMake modules aren't with the libs
IF (NOT SFML_ROOT)
	SET(SFML_ROOT ${LIB_ROOT})
ENDIF ()
SET(CMAKE_MODULE_PATH "${SFML_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})
IF (WIN32)
	# use static sfml libs on Windows
	SET(SFML_STATIC_LIBRARIES TRUE)
ENDIF ()
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
ADD_EXECUTABLE(${EXECUTABLE_NAME} ${CONTRIB_SOURCE_FILES} ${SOURCE_FILES})

# link with local objects and SFML libs & dependencies
TARGET_LINK_LIBRARIES(
		${EXECUTABLE_NAME}
		${EXTERNAL_LIBS}
)
