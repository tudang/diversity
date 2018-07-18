cmake_minimum_required (VERSION 2.8.4)
project (p4xos)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package (dpdk REQUIRED)
include_directories (${DPDK_INCLUDE_DIR})

set (RTE_TARGET x86_64-native-linuxapp-gcc)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mssse3 -O0 -Wall -Wextra -Wno-unused-parameter -ggdb -g")

set (DEP_LIBS ${DEP_LIBS} ${DPDK_LIBRARIES} m)
aux_source_directory (src SRCS)
set (TARGET dpdk_server)

add_executable (${TARGET} ${SRCS})
target_link_libraries (${TARGET}
    ${DEP_LIBS})
set_target_properties (${TARGET} PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
