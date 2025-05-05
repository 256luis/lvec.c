cmake_minimum_required(VERSION 3.10)
project(list)

include_directories(include)

set(SOURCES
    src/list.c)

set(HEADERS
    include/list.h)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

add_library(${PROJECT_NAME}
            ${SOURCES}
            ${HEADERS})
