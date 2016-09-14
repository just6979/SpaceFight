cmake_minimum_required(VERSION 3.3)

project("SpaceFight")

set(EXECUTABLE_NAME ${PROJECT_NAME})

# build locally; not somewhere hidden in %APPDATA%, as the CLion default does
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/)

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

# local def to hide debugging code for release builds
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-DDO_DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# use ccache if found
find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

# add local headers
INCLUDE_DIRECTORIES(include)
file(GLOB HEADER_FILES include/*.h)

# add all source files
file(GLOB SOURCE_FILES src/*.cpp)

# add headers and source for inih
INCLUDE_DIRECTORIES(contrib/inih)
file(GLOB CONTRIB_FILES ${CONTRIB_FILES} contrib/inih/*.c contrib/inih/cpp/*.cpp)
# skip any tests (may be removed soon if changes to inih are accepted)
file(GLOB_RECURSE SKIP_FILES contrib/inih "*test*")
message(STATUS ${SKIP_FILES})
list(REMOVE_ITEM CONTRIB_FILES ${SKIP_FILES})

# add headers and source for logog
INCLUDE_DIRECTORIES(contrib/logog/include)
file(GLOB CONTRIB_FILES ${CONTRIB_FILES} contrib/logog/src/*.cpp)

# add output binary dependent on local headers & sources and contrib sources
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${CONTRIB_FILES})

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

# link with local objects and SFML libs & dependencies
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
