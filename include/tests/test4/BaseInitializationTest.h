#pragma once

#include <glm/vec4.hpp>

#include <vector>

namespace tests {
class BaseInitializationTest
{
  public:
    BaseInitializationTest() = default;
    virtual ~BaseInitializationTest() = default;

  protected:
    const std::vector<glm::vec4>& vertices() const;
};
}
