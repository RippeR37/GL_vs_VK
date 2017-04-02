#pragma once

#include <framework/VKTest.h>

namespace tests {
namespace vk {
class SimpleBallsSceneTest : public framework::VKTest
{
  public:
    SimpleBallsSceneTest();

    void run() override;
};
}
}
