cmake_minimum_required (VERSION 2.8.4)
project (profiling)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-unused-parameter -ggdb -g")

add_executable (run_test src/main.c)
set_target_properties (run_test PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
