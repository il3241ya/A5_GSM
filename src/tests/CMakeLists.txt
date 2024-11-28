cmake_minimum_required(VERSION 3.19)

set(SOURCES test.cpp)

add_executable(test ${SOURCES})

target_link_libraries(test PRIVATE a5-gsm GTest::gtest_main)

target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR})
