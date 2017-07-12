#pragma once

/**
 *
 * Test #3 - shadow mapping scene
 *
 * In this test we render a "checkboard" floor with differently-colored cubes and above that we
 * render one high-res sphere and many cubes in different positions. Rendering happens in two passes:
 * - depth pass from light PoV to acquire shadowmap,
 * - real render pass which simply renderes scene shadowing necessary fragments.
 *
 * This test allows for some configuration due to constants defined in
 * `BaseShadowMappingSceneTest.cpp` file, such as:
 * - shadowmap texture size,
 * - position and direction of light source.
 * Both of these values are tuned to work nicely, so it's recommended to not change them.
 *
 * OpenGL multithreaded version, due to lack of heavy CPU-related workload, is absent.
 * Vulkan multithreaded version schedules building secondary command buffers on all available threads.
 *
 */

#include <tests/test3/gl/ShadowMappingSceneTest.h>
#include <tests/test3/vk/MultithreadedShadowMappingSceneTest.h>
#include <tests/test3/vk/ShadowMappingSceneTest.h>
