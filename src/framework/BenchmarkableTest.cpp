#include <framework/BenchmarkableTest.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>

namespace framework {
BenchmarkableTest::BenchmarkableTest(bool benchmarkMode, float benchmarkTime)
    : TestInterface()
    , _benchmarkEnabled(benchmarkMode)
    , _firstSecondIgnored(false)
    , _benchmarkTime(benchmarkTime)
    , _minFrameTime(std::numeric_limits<double>::max())
    , _maxFrameTime(0.0)
    , _startTime(0.0)
    , _measuredTime(0.0)
    , _frameCount(0u)
{
}

void BenchmarkableTest::printStatistics() const
{
    if (!_benchmarkEnabled)
        return;

    auto toFps = [](double frameTime) -> std::string {
        if (frameTime <= 0.0) {
            return "---";
        } else {
            return std::to_string(1.0 / frameTime);
        }
    };

    auto toMs = [](double frameTime) -> std::string { return std::to_string(frameTime * 1000.0) + "ms"; };

    std::cout << "Frame rate statistics" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "  Minimum frame time: " << toMs(_minFrameTime) << std::endl;
    std::cout << "  Maximum frame time: " << toMs(_maxFrameTime) << std::endl;
    std::cout << "  Average frame time: " << toMs(_measuredTime / static_cast<double>(_frameCount)) << std::endl;
    std::cout << std::endl;
    std::cout << "  Maximum FPS: " << toFps(_minFrameTime) << std::endl;
    std::cout << "  Minimum FPS: " << toFps(_maxFrameTime) << std::endl;
    std::cout << "  Average FPS: " << toFps(_measuredTime / static_cast<double>(_frameCount)) << std::endl;
}

void BenchmarkableTest::startMeasuring()
{
    if (!_benchmarkEnabled)
        return;

    _startTime = glfwGetTime();
    _frameCount = 0u;
}

bool BenchmarkableTest::processFrameTime(double frameTime)
{
    if (!_benchmarkEnabled)
        return false;

    ++_frameCount;
    _minFrameTime = std::min(_minFrameTime, frameTime);
    _maxFrameTime = std::max(_maxFrameTime, frameTime);
    _measuredTime = glfwGetTime() - _startTime;

    // Ignore 1s of measurements to remove longer first frames from statistics
    if (!_firstSecondIgnored && _measuredTime >= 1.0) {
        startMeasuring();
        _firstSecondIgnored = true;
    }

    return (_measuredTime >= _benchmarkTime);
}
}
