#include <framework/VKTest.h>

namespace framework {
VKTest::VKTest(const std::string& testName)
    : TestInterface()
    , vkx::Application("[VK] " + testName, {WINDOW_WIDTH, WINDOW_HEIGHT}, true)
{
}

void VKTest::setup()
{
}

void VKTest::teardown()
{
}
}
