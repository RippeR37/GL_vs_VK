#pragma once

#include <base/gl/Window.h>
#include <framework/TestInterface.h>

namespace framework {
class GLTest : public TestInterface
{
  public:
    GLTest(const std::string& testName);
    virtual ~GLTest() = default;

    virtual void setup() override;
    virtual void teardown() override;

  protected:
    base::gl::Window window_;
};
}
