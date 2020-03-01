cmake_minimum_required(VERSION 3.10)
project(s9r)
enable_testing()

set(CMAKE_CXX_STANDARD, 11)

# Init variables
set(LINKLIBS)

# Build libsoundio
set(BUILD_EXAMPLE_PROGRAMS OFF)
set(BUILD_TESTS OFF)
add_subdirectory(lib/libsoundio)
include_directories(lib/libsoundio)
link_directories(lib/libsoundio)
list(APPEND LINKLIBS libsoundio_shared)

# add RtMidi
include_directories(${PROJECT_SOURCE_DIR}/lib/rtmidi)
link_directories(${PROJECT_SOURCE_DIR}/lib/rtmidi)
add_library(rtmidi STATIC ${PROJECT_SOURCE_DIR}/lib/rtmidi/RtMidi.cpp)
list(APPEND LINKLIBS rtmidi)

if(UNIX AND NOT APPLE)
  add_definitions(-D__UNIX_JACK__)
  list(APPEND LINKLIBS jack pthread m)
endif()

if(WIN32)
  add_definitions(-D__WINDOWS_MM__)
  list(APPEND API_DEFS "-D__WINDOWS_MM__")
  list(APPEND LINKLIBS winmm)
endif()

# add gl3w
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/lib/gl3w/src/gl3w.c)
  execute_process(
    COMMAND python3 ${PROJECT_SOURCE_DIR}/lib/gl3w/gl3w_gen.py
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/gl3w
    )
endif()
#add_subdirectory(${PROJECT_SOURCE_DIR}/lib/gl3w)

add_library(gl3w STATIC ${PROJECT_SOURCE_DIR}/lib/gl3w/src/gl3w.c)
if(UNIX AND NOT APPLE)
  target_link_libraries(gl3w dl)
endif()

include_directories(${PROJECT_SOURCE_DIR}/lib/gl3w/include)
link_directories(${PROJECT_SOURCE_DIR}/lib/gl3w)
list(APPEND LINKLIBS gl3w)  # gl3w requires dl

# add GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
set(GLFW_USE_CHDIR OFF CACHE BOOL " " FORCE)
#set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glfw)
include_directories(${PROJECT_SOURCE_DIR}/lib/glfw/include)
list(APPEND LINKLIBS glfw)

# add nanovg
if (CMAKE_COMPILER_IS_GNUCC)
  set_source_files_properties(lib/nanovg/src/nanovg.c PROPERTIES COMPILE_FLAGS -Wno-unused-result)
elseif(MSVC)
  set_source_files_properties(lib/nanovg/src/nanovg.c PROPERTIES COMPILE_FLAGS "/wd4005 /wd4456 /wd4457")
endif()
include_directories(${PROJECT_SOURCE_DIR}/lib/nanovg/src)
add_library(nanovg STATIC ${PROJECT_SOURCE_DIR}/lib//nanovg/src/nanovg.c)
list(APPEND LINKLIBS nanovg)

# for Linux
if(UNIX AND NOT APPLE)
  list(APPEND LINKLIBS jack pthread m)
endif()

# add fmt
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/fmt)
include_directories(${PROJECT_SOURCE_DIR}/lib/fmt)
link_directories(${PROJECT_SOURCE_DIR}/lib/fmt)
list(APPEND LINKLIBS fmt)

# s9r
# set MY_SRCS(it is testing target)
file(GLOB MY_SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${MY_SRCS})
target_link_libraries(${PROJECT_NAME} ${LINKLIBS})

# add tests
add_subdirectory(tests)
