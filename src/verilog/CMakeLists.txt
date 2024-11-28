cmake_minimum_required(VERSION 3.19)

set(SOURCES a5-cipher.v rc-loss.v)

add_executable(v-sim sim.cpp)

target_link_libraries(v-sim PRIVATE a5-gsm)

target_include_directories(v-sim PRIVATE ${CMAKE_SOURCE_DIR})

verilate(v-sim SOURCES ${SOURCES} PREFIX vcipher THREADS 8)