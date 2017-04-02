#pragma once

#include <framework/TestInterface.h>

#include <string>

namespace framework {
class VKTest : public TestInterface
{
  public:
    VKTest(std::string testName);

    virtual void setup() override;
    virtual void teardown() override;

  protected:
    //
};
}
