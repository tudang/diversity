cmake_minimum_required (VERSION 2.6)
project (TestHashmap)
# include_directories("${PROJECT_SOURCE_DIR}/include")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11")

file(GLOB SOURCES src/*.cc)

#find_package(Boost COMPONENTS unordered_map REQUIRED)
#include_directories (${Boost_INCLUDE_DIRS})
#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)
#add_definitions (-DBOOST_TEST_DYN_LINK)
#set (EXTRA_LIBS ${EXTRA_LIBS}  ${Boost_LIBRARIES})

set (EXTRA_LIBS ${EXTRA_LIBS})

add_executable(Runner ${SOURCES})
target_link_libraries (Runner ${EXTRA_LIBS})
set_target_properties (Runner PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})