cmake_minimum_required(VERSION 3.10)

add_executable(${PROJECT_NAME} ${MY_SRCS})
target_link_libraries(${PROJECT_NAME} ${LINKLIBS})
