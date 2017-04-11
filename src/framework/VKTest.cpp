#include <framework/VKTest.h>

namespace framework {
VKTest::VKTest(const std::string& testName)
    : TestInterface()
    , vkx::Application("[VK] " + testName, true)
{
}

void VKTest::setup()
{
}

void VKTest::teardown()
{
}
}
