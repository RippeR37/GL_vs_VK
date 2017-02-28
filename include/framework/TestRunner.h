#pragma once

#include <base/ArgumentParser.h>

namespace framework {
class TestRunner {
public:
    TestRunner(base::ArgumentParser argumentParser);

    int run();

private:
    int run_gl(int testNumber);
    int run_vk(int testNumber);

    base::ArgumentParser arguments;
};
}
