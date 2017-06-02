#pragma once

#include <base/gl/Window.h>
#include <framework/BenchmarkableTest.h>

namespace framework {
class GLTest : public BenchmarkableTest
{
  public:
    GLTest(const std::string& testName, bool benchmarkMode, float benchmarkTime);
    virtual ~GLTest() = default;

    virtual void setup() override;
    virtual void teardown() override;

    void printStatistics() const override;

  protected:
    base::gl::Window window_;
};
}
