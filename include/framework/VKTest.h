#pragma once

#include <base/vkx/Application.h>
#include <framework/TestInterface.h>

#include <string>

namespace framework {
class VKTest : public TestInterface, base::vkx::Application
{
  public:
    VKTest(const std::string& testName);

    virtual void setup() override;
    virtual void teardown() override;

  protected:
    //
};
}
