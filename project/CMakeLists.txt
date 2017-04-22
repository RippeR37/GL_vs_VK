###############################################
# Compiler flags
###############################################

enable_language(CXX)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(WARNINGS "-Wall -Wextra -Werror")
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

find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

set(GLvsVK_LINK_LIBRARIES
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${Vulkan_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
)

set(GLvsVK_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIR}
    ${Vulkan_INCLUDE_DIRS}
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
