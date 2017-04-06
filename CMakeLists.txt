###############################################
# CMake options
###############################################

cmake_minimum_required (VERSION 3.7.2 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/")


###############################################
# GLvsVK
###############################################

add_subdirectory(project)
