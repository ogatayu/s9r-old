cmake_minimum_required(VERSION 3.7.2)

# Init variables
set(LINKLIBS)

# for Linux
if(UNIX AND NOT APPLE)
  add_definitions(-D__UNIX_JACK__)
  list(APPEND LINKLIBS jack pthread soundio m)
endif()

include_directories(lib/rtmidi)
link_directories(lib/rtmidi)

add_executable(s9r src/main.cpp src/synth.cpp src/midi.cpp lib/rtmidi/RtMidi.cpp)
target_link_libraries(s9r ${LINKLIBS})
