#pragma once

/**
 *
 * Test #1 - static scene
 *
 * This test resolves around single static scene with variable number of rendered objects
 * which quality can be chosen (each object is a sphere made of specified ammount of vertices).
 *
 * This test have several configuration options that are available in `BaseBallsSceneTest.cpp`
 * file, such as:
 * - number of rendered objects,
 * - number of vertices per sphere (slices and stacks),
 * - ammount of update work per each object in each frame.
 * This was added to simulate different ammount of CPU and GPU work and to emulate CPU- and
 * GPU-bound scenarios.
 *
 * Implementation is rather simple and consists of simple draw calls per each object, to emulate
 * scenario where lots of different objects are rendered. Instancing is not used.
 *
 * OpenGL multithreaded version schedules update work on all available threads.
 * Vulkan multithreaded version schedules both update work and building command buffers on all
 * available threads.
 *
 */

#include <tests/test1/gl/MultithreadedBallsSceneTest.h>
#include <tests/test1/gl/SimpleBallsSceneTest.h>
#include <tests/test1/vk/MultithreadedBallsSceneTest.h>
#include <tests/test1/vk/SimpleBallsSceneTest.h>
