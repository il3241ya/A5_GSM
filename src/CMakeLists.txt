cmake_minimum_required(VERSION 3.19)

set(SOURCES solver.c test-suite-gen.c)
set(HEADERS solver.h test-suite-gen.h)

add_library(a5-gsm STATIC ${SOURCES} ${HEADERS})

add_subdirectory(tests)
add_subdirectory(verilog)
