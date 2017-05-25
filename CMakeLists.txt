###############################################
# CMake options
###############################################

cmake_minimum_required (VERSION 3.5.1 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/")


###############################################
# GLvsVK
###############################################

add_subdirectory(project)
