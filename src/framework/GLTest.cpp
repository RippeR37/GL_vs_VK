#include <framework/GLTest.h>

namespace framework {
GLTest::GLTest(const std::string& testName)
    : TestInterface()
    , window_({WINDOW_WIDTH, WINDOW_HEIGHT}, "[GL] " + testName)
{
}

void GLTest::setup()
{
    window_.create();
    window_.disableVSync();
}

void GLTest::teardown()
{
    window_.destroy();
    window_.deinitialize();
}
}
