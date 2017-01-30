cmake_minimum_required(VERSION 2.8)

project(tests C)

SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
add_executable(utf8test utf8test.c)
target_link_libraries(utf8test utf8string)
