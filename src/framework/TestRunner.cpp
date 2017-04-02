#include <base/gl/Window.h>
#include <framework/TestRunner.h>
#include <tests/test1/SimpleBallsSceneTest.h>

#include <iostream>
#include <stdexcept>

namespace framework {
TestRunner::TestRunner(base::ArgumentParser argumentParser) : arguments(std::move(argumentParser))
{
}

int TestRunner::run()
{
    const int TESTS = 1;

    auto errorCallback = [&](const std::string& msg) -> int {
        std::cerr << "Invalid usage! " << msg << std::endl;
        std::cerr << "Usage: `" << arguments.getPath() << " -t T -api API`" << std::endl;
        std::cerr << "  T     - test number (in range [1, " << TESTS << "])" << std::endl;
        std::cerr << "  API   - API (`gl` or `vk`)" << std::endl;
        return -1;
    };

    if (!arguments.hasArgument("t"))
        return errorCallback("Missing `-t` argument!");

    if (!arguments.hasArgument("api"))
        return errorCallback("Missing `-api` argument!");

    int testNum = -1;
    try {
        testNum = arguments.getIntArgument("t");
    } catch (...) {
        // ignore, will fail with proper message later
    }

    std::string api = arguments.getArgument("api");

    if (testNum < 1 || testNum > TESTS)
        return errorCallback("Invalid test number!");

    if (api != "gl" && api != "vk")
        return errorCallback("Invalid `-api` value!");

    if (api == "gl") {
        return run_gl(testNum);
    } else {
        return run_vk(testNum);
    }
}

int TestRunner::run_gl(int testNumber)
{
    std::unique_ptr<TestInterface> test;

    switch (testNumber) {
    case 1:
        test = std::unique_ptr<TestInterface>(new tests::gl::SimpleBallsSceneTest());
        break;
    }

    if (test) {
        return run_any(std::move(test));
    } else {
        std::cerr << "Unknown OpenGL test: " << testNumber << std::endl;
        return -1;
    }
}

int TestRunner::run_vk(int testNumber)
{
    std::unique_ptr<TestInterface> test;

    switch (testNumber) {
    case 1:
        test = std::unique_ptr<TestInterface>(new tests::vk::SimpleBallsSceneTest());
        break;
    }

    if (test) {
        return run_any(std::move(test));
    } else {
        std::cerr << "Unknown Vulkan test: " << testNumber << std::endl;
        return -1;
    }
}

int TestRunner::run_any(std::unique_ptr<TestInterface> test)
{
    try {
        test->setup();
        test->run();
        test->teardown();

    } catch (const std::runtime_error& exception) {
        std::cerr << "Caught runtime exception during test execution!" << std::endl;
        std::cerr << exception.what() << std::endl;
        return -1;

    } catch (const std::exception& exception) {
        std::cerr << "Caught exception during test execution!" << std::endl;
        std::cerr << exception.what() << std::endl;
        return -1;

    } catch (...) {
        std::cerr << "Caught unknown exception during test execution!" << std::endl;
        return -1;
    }

    return 0;
}
}
