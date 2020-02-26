cmake_minimum_required(VERSION 3.10)
project(s9r)

# Init variables
set(LINKLIBS)

# install libsoundio
set(BUILD_DYNAMIC_LIBS OFF)
set(BUILD_EXAMPLE_PROGRAMS OFF)
set(BUILD_TESTS OFF)
set(ENABLE_PULSEAUDIO OFF)
set(ENABLE_ALSA OFF)
add_subdirectory(lib/libsoundio)
link_directories(lib/libsoundio)
list(APPEND LINKLIBS libsoundio_static)

# for Linux
if(UNIX AND NOT APPLE)
  add_definitions(-D__UNIX_JACK__)
  list(APPEND LINKLIBS jack pthread m)
endif()

# RtMidi
include_directories(${PROJECT_SOURCE_DIR}/lib/rtmidi)
link_directories(${PROJECT_SOURCE_DIR}/lib/rtmidi)
add_library(rtmidi STATIC ${PROJECT_SOURCE_DIR}/lib/rtmidi/RtMidi.cpp)
list(APPEND LINKLIBS rtmidi)

# set PROJECT_SOURCE_DIR
file(GLOB MY_SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
