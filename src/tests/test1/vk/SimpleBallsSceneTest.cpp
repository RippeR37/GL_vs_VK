#include <tests/test1/vk/SimpleBallsSceneTest.h>

#include <vulkan/vulkan.hpp>

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace tests {
namespace test_vk {
SimpleBallsSceneTest::SimpleBallsSceneTest()
    : VKTest("SimpleBallsSceneTest")
{
}

void SimpleBallsSceneTest::run()
{
    while (!window().shouldClose()) {
        // rendering

        window().update();
    }
}
}
}
