#pragma once

/**
 *
 * Test #2 - terrain scene with dynamic Level-of-Detail
 *
 * In this test, a terrain is rendered (as wireframe) with active dynamic level-of-detail changes.
 * Each vertex is colored in fragment shader with regard to it's height. At startup, it loads
 * 1024x1024 heightmap and pre-computes any necessary data. At runtime it traverses quad-tree
 * structure and decides which nodes should be rendered with current resolution, or if map's
 * segment should be rendered with higher resoltuion and thus the node should be split.
 *
 * This test allows for some configuration due to constants defined in `BaseTerrainSceneTest.cpp`
 * file, such as:
 * - heightmap size (requires valid heightmap file present in given location!),
 * - level-of-detail change factor.
 * Both of these values are tuned to work nicely, so it's recommended to not change them.
 *
 * OpenGL multithreaded version, due to lack of heavy CPU-related workload, is absent.
 * Vulkan multithreaded version schedules traversal of quad-tree and building secondary
 * command buffers on 4 threads.
 *
 */

#include <tests/test2/gl/TerrainSceneTest.h>
#include <tests/test2/vk/MultithreadedTerrainSceneTest.h>
#include <tests/test2/vk/TerrainSceneTest.h>
