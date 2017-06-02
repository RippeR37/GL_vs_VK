#include <framework/GLTest.h>

#include <GL/glew.h>

#include <iostream>

namespace framework {
GLTest::GLTest(const std::string& testName, bool benchmarkMode, float benchmarkTime)
    : BenchmarkableTest(benchmarkMode, benchmarkTime)
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

void GLTest::printStatistics() const
{
    std::cout << "Hardware/software information" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "  Vendor:     " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "  Renderer:   " << glGetString(GL_RENDERER) << std::endl;
    std::cout << std::endl;

    BenchmarkableTest::printStatistics();
}
}
