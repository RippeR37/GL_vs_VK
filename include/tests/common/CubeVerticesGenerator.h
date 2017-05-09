#pragma once

#include <glm/vec4.hpp>

#include <vector>

namespace tests {
namespace common {
class CubeVerticesGenerator
{
  public:
    CubeVerticesGenerator();
    CubeVerticesGenerator(const CubeVerticesGenerator&) = delete;

    CubeVerticesGenerator& operator=(const CubeVerticesGenerator&) = delete;

    const std::vector<glm::vec4> vertices;
    const std::vector<glm::vec4> normals;

  private:
    std::vector<glm::vec4> generateVertices();
    std::vector<glm::vec4> generateNormals();
};
}
}
