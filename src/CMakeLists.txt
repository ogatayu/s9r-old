cmake_minimum_required(VERSION 3.10)

# Init variables
set(LINKLIBS)

# for Linux
if(UNIX AND NOT APPLE)
  add_definitions(-D__UNIX_JACK__)
  list(APPEND LINKLIBS jack pthread soundio m)
endif()

# RtMidi
include_directories(${PROJECT_SOURCE_DIR}/lib/rtmidi)
link_directories(${PROJECT_SOURCE_DIR}/lib/rtmidi)
add_library(rtmidi STATIC ${PROJECT_SOURCE_DIR}/lib/rtmidi/RtMidi.cpp)

add_executable(
    s9r
    main.cpp
    synth.cpp
    midi.cpp
    )

target_link_libraries(s9r rtmidi ${LINKLIBS})
