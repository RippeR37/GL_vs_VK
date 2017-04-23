#pragma once

#include <base/ArgumentParser.h>
#include <framework/TestInterface.h>

#include <memory>

namespace framework {
class TestRunner
{
  public:
    TestRunner(base::ArgumentParser argumentParser);

    int run();

  private:
    int run_gl(int testNumber);
    int run_vk(int testNumber, bool multithreaded);
    int run_any(std::unique_ptr<TestInterface> test);

    base::ArgumentParser arguments;
};
}
