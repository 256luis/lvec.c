cmake_minimum_required(VERSION 3.15)
project(lvec)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

add_library(${PROJECT_NAME} STATIC
            src/lvec.c
            include/lvec.h)

target_include_directories(${PROJECT_NAME} PUBLIC
                           include)
