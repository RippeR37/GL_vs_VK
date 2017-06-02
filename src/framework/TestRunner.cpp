#include <base/gl/Window.h>
#include <framework/TestRunner.h>
#include <tests/test1/BallsSceneTests.h>
#include <tests/test2/TerrainSceneTests.h>
#include <tests/test3/ShadowMappingSceneTests.h>

#include <iostream>
#include <stdexcept>

namespace {
const float kDefaultTestBenchmarkTime = 15.0f; // 15 seconds
}

namespace framework {
TestRunner::TestRunner(base::ArgumentParser argumentParser)
    : arguments(std::move(argumentParser))
{
}

int TestRunner::run()
{
    const int TESTS = 3;

    auto errorCallback = [&](const std::string& msg) -> int {
        std::cerr << "Invalid usage! " << msg << std::endl;
        std::cerr << "Usage: `" << arguments.getPath() << " -t T -api API [-m]`" << std::endl;
        std::cerr << "  -t T       - test number (in range [1, " << TESTS << "])" << std::endl;
        std::cerr << "  -api API   - API (`gl` or `vk`)" << std::endl;
        std::cerr << "  -m         - run multithreaded version (if exists)" << std::endl;
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

    if (testNum < 1 || testNum > TESTS)
        return errorCallback("Invalid test number!");

    std::string api = arguments.getArgument("api");
    if (api != "gl" && api != "vk")
        return errorCallback("Invalid `-api` value!");

    bool multithreaded = arguments.hasArgument("m");
    bool benchmarkMode = arguments.hasArgument("benchmark");
    float benchmarkTime = kDefaultTestBenchmarkTime;

    if (benchmarkMode && arguments.hasArgument("time")) {
        try {
            benchmarkTime = arguments.getFloatArgument("time");
        } catch (...) {
            std::cerr << "Invalid value for argument '-time'!";
        }
    }

    if (api == "gl") {
        return run_gl(testNum, multithreaded, benchmarkMode, benchmarkTime);
    } else {
        return run_vk(testNum, multithreaded, benchmarkMode, benchmarkTime);
    }
}

int TestRunner::run_gl(int testNumber, bool multithreaded, bool benchmarkMode, float benchmarkTime)
{
    std::unique_ptr<BenchmarkableTest> test;

    switch (testNumber) {
    case 1:
        if (multithreaded) {
            test = std::unique_ptr<BenchmarkableTest>(
                new tests::test_gl::MultithreadedBallsSceneTest(benchmarkMode, benchmarkTime));
        } else {
            test = std::unique_ptr<BenchmarkableTest>(
                new tests::test_gl::SimpleBallsSceneTest(benchmarkMode, benchmarkTime));
        }
        break;

    case 2:
        if (multithreaded) {
            // N/A
        } else {
            test =
                std::unique_ptr<BenchmarkableTest>(new tests::test_gl::TerrainSceneTest(benchmarkMode, benchmarkTime));
        }
        break;

    case 3:
        if (multithreaded) {
            // N/A
        } else {
            test = std::unique_ptr<BenchmarkableTest>(
                new tests::test_gl::ShadowMappingSceneTest(benchmarkMode, benchmarkTime));
        }
        break;
    }

    if (test) {
        return run_any(std::move(test));
    } else {
        std::cerr << "Unknown " << (multithreaded ? "multithreaded" : "") << " OpenGL test: " << testNumber
                  << std::endl;
        return -1;
    }
}

int TestRunner::run_vk(int testNumber, bool multithreaded, bool benchmarkMode, float benchmarkTime)
{
    std::unique_ptr<BenchmarkableTest> test;

    switch (testNumber) {
    case 1:
        if (multithreaded) {
            test = std::unique_ptr<BenchmarkableTest>(
                new tests::test_vk::MultithreadedBallsSceneTest(benchmarkMode, benchmarkTime));
        } else {
            test = std::unique_ptr<BenchmarkableTest>(
                new tests::test_vk::SimpleBallsSceneTest(benchmarkMode, benchmarkTime));
        }
        break;

    case 2:
        if (multithreaded) {
            test = std::unique_ptr<BenchmarkableTest>(
                new tests::test_vk::MultithreadedTerrainSceneTest(benchmarkMode, benchmarkTime));
        } else {
            test =
                std::unique_ptr<BenchmarkableTest>(new tests::test_vk::TerrainSceneTest(benchmarkMode, benchmarkTime));
        }
        break;

    case 3:
        if (multithreaded) {
            test = std::unique_ptr<BenchmarkableTest>(
                new tests::test_vk::MultithreadedShadowMappingSceneTest(benchmarkMode, benchmarkTime));
        } else {
            test = std::unique_ptr<BenchmarkableTest>(
                new tests::test_vk::ShadowMappingSceneTest(benchmarkMode, benchmarkTime));
        }
        break;
    }

    if (test) {
        return run_any(std::move(test));
    } else {
        std::cerr << "Unknown " << (multithreaded ? "multithreaded" : "") << " Vulkan test: " << testNumber
                  << std::endl;
        return -1;
    }
}

int TestRunner::run_any(std::unique_ptr<BenchmarkableTest> test)
{
    try {
        test->setup();
        test->startMeasuring();
        test->run();
        test->printStatistics();
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
