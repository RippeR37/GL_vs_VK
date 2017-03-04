#include <framework/GLTest.h>

namespace framework {
GLTest::GLTest(std::string testName)
    : TestInterface()
    , window_({ WINDOW_WIDTH, WINDOW_HEIGHT }, "[GL] " + testName)
{
}

void GLTest::setup()
{
    window_.create();
}

void GLTest::teardown()
{
    window_.destroy();
    window_.deinitialize();
}
}
