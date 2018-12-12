cmake_minimum_required (VERSION 2.6)
project (TestMultithread)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-unused-parameter")

set (EXTRA_LIBS pthread)

file(GLOB SOURCES src/*.c)

add_executable(Runner ${SOURCES})
target_link_libraries (Runner ${EXTRA_LIBS})
set_target_properties (Runner PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
