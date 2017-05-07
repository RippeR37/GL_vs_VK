#pragma once

#include <tests/common/TerrainLoD.h>

#include <glm/matrix.hpp>

namespace tests {
class BaseTerrainSceneTest
{
  public:
    BaseTerrainSceneTest();
    virtual ~BaseTerrainSceneTest() = default;

  protected:
    void updateTestState(double dt);

    const common::TerrainLoD& terrain() const;
    const glm::vec2& currentPosition() const;
    const glm::mat4& currentMVP() const;

  private:
    common::TerrainLoD _terrain;
    glm::vec2 _position;
};
}
