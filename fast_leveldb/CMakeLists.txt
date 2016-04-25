cmake_minimum_required (VERSION 2.8.4)
project (MillionPackets)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-unused-parameter -ggdb -g")

# print message when running cmake
message(STATUS "This is CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS})

# leveldb
include_directories(/usr/local/include/leveldb/include)
link_directories(/usr/local/lib)

add_executable(sender src/udpsender.c src/net.c)
target_link_libraries(sender leveldb pthread)
SET_TARGET_PROPERTIES(sender PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
add_executable(receiver src/udpreceiver.c src/net.c)
target_link_libraries(receiver leveldb pthread)
SET_TARGET_PROPERTIES(receiver PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})