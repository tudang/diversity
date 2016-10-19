cmake_minimum_required (VERSION 2.8.4)
project (dummy)

include_directories(${CMAKE_SOURCE_DIR}/include)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-unused-parameter -ggdb -g")

aux_source_directory (src SRCS)
message (STATUS "This is CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS})

foreach (SRC_PATH ${SRCS})
    get_filename_component(SRC ${SRC_PATH} NAME_WE)
    message (${SRC})
    add_library (${SRC} SHARED ${SRCS})
    set_target_properties (${SRC} PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
endforeach(SRC_PATH)
