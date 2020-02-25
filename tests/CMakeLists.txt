cmake_minimum_required(VERSION 3.10)

include(${PROJECT_SOURCE_DIR}/cmake/DownloadProject.cmake)
download_project(PROJ googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG master
                UPDATE_DISCONNECTED 1
)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# lists poduct source
set(MY_SRCS_MINUS_MAIN ${MY_SRCS})
list(REMOVE_ITEM MY_SRCS_MINUS_MAIN ${PROJECT_SOURCE_DIR}/src/main.cpp)

# lists test source
file(GLOB MY_TEST_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(
    gtestExecutor

    ${MY_TEST_SRCS}
    ${MY_SRCS_MINUS_MAIN}
    )

target_link_libraries(gtestExecutor gtest_main ${LINKLIBS})
include_directories(${PROJECT_SOURCE_DIR}/src)

include(GoogleTest)
gtest_add_tests(TARGET gtestExecutor)
