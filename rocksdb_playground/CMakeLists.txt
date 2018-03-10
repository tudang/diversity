cmake_minimum_required (VERSION 2.8.4)
project (dummy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

message(${CMAKE_MODULE_PATH})

find_package(rocksdb REQUIRED)
find_package(Readline)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${ROCKSDB_INCLUDE_DIRS})
include_directories(${Readline_INCLUDE_DIR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-unused-parameter -ggdb -g")

set (LIBS ${LIBS} ${ROCKSDB_LIBRARIES} ${Readline_LIBRARY})
aux_source_directory (src SRCS)
message (STATUS "This is CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS})

add_executable(run ${SRCS})
target_link_libraries (run ${LIBS})
set_target_properties (run PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
