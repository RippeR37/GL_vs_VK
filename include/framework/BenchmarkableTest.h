#pragma once

#include <framework/TestInterface.h>

#include <cstddef>

namespace framework {
class BenchmarkableTest : public TestInterface
{
  public:
    BenchmarkableTest(bool benchmarkMode, float benchmarkTime);
    virtual ~BenchmarkableTest() = default;

    virtual void printStatistics() const;
    void startMeasuring();

  protected:
    bool processFrameTime(double frameTime);

    bool _benchmarkEnabled;
    bool _firstSecondIgnored;
    double _benchmarkTime;
    double _minFrameTime;
    double _maxFrameTime;
    double _startTime;
    double _measuredTime;
    std::size_t _frameCount;
};
}
