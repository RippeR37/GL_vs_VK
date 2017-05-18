#pragma once

#include <tests/common/RenderObject.h>

#include <vector>

namespace tests {
class BaseShadowMappingSceneTest
{
  public:
    BaseShadowMappingSceneTest();
    virtual ~BaseShadowMappingSceneTest() = default;

  protected:
    void updateTestState(double dt);

    const std::vector<common::RenderObject>& renderObjects() const;
    const glm::mat4& renderMatrix() const;
    const glm::mat4& shadowMatrix() const;
    const glm::uvec2& shadowmapSize() const;

    glm::mat4 applyDepthBias(const glm::mat4& matrix) const;

  private:
    void initMatrices();
    void createRenderObjects();

    std::vector<common::RenderObject> _renderObjects;
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;
    glm::mat4 _shadowMatrix;
};
}
