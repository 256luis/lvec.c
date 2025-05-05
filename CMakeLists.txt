cmake_minimum_required(VERSION 3.15)
project(lvec)

include_directories(include)

set(SOURCES
    src/lvec.c)

set(HEADERS
    include/lvec.h)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

add_library(${PROJECT_NAME} STATIC
            ${SOURCES}
            ${HEADERS})
