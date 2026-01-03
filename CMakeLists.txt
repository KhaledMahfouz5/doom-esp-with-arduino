# Root CMakeLists.txt

cmake_minimum_required(VERSION 3.5)

# Set the project name
set(PROJECT_NAME "doom-esp-with-arduino")

# Include ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(${PROJECT_NAME})
