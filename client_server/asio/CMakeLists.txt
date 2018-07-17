cmake_minimum_required (VERSION 2.8.4)
project (clientServer)

find_package(Boost REQUIRED thread system)
include_directories(${Boost_INCLUDE_DIR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wall -Wextra -ggdb -g -pthread")
message (STATUS "This is CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS})

add_executable (server src/timer_server.cc)
target_link_libraries (server ${Boost_LIBRARIES})
set_target_properties (server PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})

add_executable (client src/timer_client.cc)
target_link_libraries (client ${Boost_LIBRARIES})
set_target_properties (client PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
