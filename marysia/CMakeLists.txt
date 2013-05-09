project(btree2)

cmake_minimum_required(VERSION 2.8)

file( GLOB_RECURSE mar_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

add_library(mar SHARED ${mar_SOURCES} )

add_executable(marysia main.cpp)

target_link_libraries(marysia mar)

