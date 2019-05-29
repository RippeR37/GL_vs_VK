###############################################
# Compiler flags
###############################################

enable_language(CXX)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(WARNINGS "-Wall -Wextra")
    set(EXTRA_FLAGS "-std=c++11")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(WARNINGS "/W4 /WX /EHsc")
    set(WARNINGS_SUPPRESSED "/wd4714 /wd4018 /wd4098 /wd4099 /wd4201 /wd4290 /wd4611 /wd4996")
    set(EXTRA_FLAGS "${WARNINGS_SUPPRESSED} -O2")
endif()

if(NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${WARNINGS} ${EXTRA_FLAGS}"
    CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()


###############################################
# GLvsVK dependencies
###############################################

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)
find_package(GLM REQUIRED)
find_package(GLEW)
if (WIN32)
    find_package(GLFW3)
else()
    find_package(GLFW)
endif()


###
### Workarounds for missing dependencies
###

### GLFW 3.2
if(NOT (GLFW3_FOUND OR GLFW_FOUND))
    MESSAGE("GLFW not found, building from third_party...")

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    set(GLFW_LIBRARIES glfw)
    set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/glfw/include")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glfw ${CMAKE_CURRENT_BINARY_DIR}/../third_party/glfw)
else()
    set(GLFW_LIBRARIES glfw)
endif()

### GLEW
if(NOT GLEW_FOUND)
    MESSAGE("GLEW not found, building from third_party...")

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
    set(GLEW_LIBRARIES glew)

    set(GLEW_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/third_party/glew/include")
    execute_process(COMMAND make WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/third_party/glew/auto")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glew/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/../third_party/glew/build/cmake)
endif()


###
### Setting up dependencies
###

set(GLvsVK_LINK_LIBRARIES
    ${CMAKE_THREAD_LIBS_INIT}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${Vulkan_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
)

set(GLvsVK_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${Vulkan_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)


###############################################
# GLvsVK headers & sources
###############################################

file(GLOB_RECURSE GLvsVK_HEADERS
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
)

file(GLOB_RECURSE GLvsVK_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)


###############################################
# GLvsVK Binary
###############################################

project(GLvsVK_EXAMPLE_2D CXX)

include_directories(${GLvsVK_INCLUDE_DIRS})

add_executable(GLvsVK_EXAMPLE_2D
    ${GLvsVK_SOURCES}
    ${GLvsVK_HEADERS}
)

target_link_libraries(GLvsVK_EXAMPLE_2D
    ${GLvsVK_LINK_LIBRARIES}
)

set_target_properties(GLvsVK_EXAMPLE_2D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
    RUNTIME_OUTPUT_NAME "GL_vs_VK"
)
