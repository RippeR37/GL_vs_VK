#include <framework/VKTest.h>

namespace {
const bool kDebugEnabled = false;
}

namespace framework {
VKTest::VKTest(const std::string& testName)
    : TestInterface()
    , base::vkx::Application("[VK] " + testName, {WINDOW_WIDTH, WINDOW_HEIGHT}, kDebugEnabled)
{
}

void VKTest::setup()
{
}

void VKTest::teardown()
{
}
}
