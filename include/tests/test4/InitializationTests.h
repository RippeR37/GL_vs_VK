#pragma once

/**
 *
 * Test #4 - initialization benchmark
 *
 * Test measures time from initialization start of OpenGL/Vulkan objects up to first draw
 * (and present/swap buffer) call, synchronizes between CPU and GPU to make sure that everything
 * is executed and measures elapsed time. This is non-practical test, but I wanted to know
 * how both APIs behave. For Vulkan I've added support for pipeline cache.
 * Both versions measures time from start of application (right before touching graphics API)
 * and includes time for window creation, as some APIs might do some work at that time
 * (e.g. create default frame buffers, pipeline objects etc.).
 *
 * This test doesn't have any configuration options.
 *
 * Due to lack of heavy CPU load that can be parallelized, both OpenGL and Vulkan versions
 * doesn't have multithreaded configurations in this test.
 *
 */

#include <tests/test4/gl/InitializationTest.h>
#include <tests/test4/vk/InitializationTest.h>
