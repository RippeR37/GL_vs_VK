#pragma once

namespace framework {
class TestInterface
{
  public:
    virtual ~TestInterface() = default;

    static const unsigned int WINDOW_WIDTH = 800;
    static const unsigned int WINDOW_HEIGHT = 600;

    virtual void setup() = 0;
    virtual void run() = 0;
    virtual void teardown() = 0;
};
}
