cmake_minimum_required(VERSION 2.8)

project(utf8string C)

# make it tunable
add_definitions(-DDEBUG)

include_directories(include)

file(GLOB headers_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    include/utf8proc.h
    include/utf8string.h
    include/logger.h
)
file(GLOB sources_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/utf8proc.c
    src/utf8string.c
    src/logger.c
)

source_group("" FILES ${headers_} ${sources_})

if (WIN32)
#    add_definitions(-D_UNICODE -DUNICODE)
    set(platformDir_ Win32)
else()
    set(platformDir_ Posix)
endif(WIN32)

add_library(utf8string STATIC ${sources_})

add_subdirectory(tests)
