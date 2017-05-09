#pragma once

#include <glm/vec4.hpp>

#include <vector>

namespace tests {
namespace common {
class SphereVerticesGenerator
{
  public:
    SphereVerticesGenerator(std::size_t slices, std::size_t stacks, float radius = 1.0f);

    SphereVerticesGenerator& operator=(const SphereVerticesGenerator&) = delete;

    const std::vector<glm::vec4> vertices;
    const std::vector<glm::vec4> normals;

  private:
    std::vector<glm::vec4> generateVertices(std::size_t slices, std::size_t stacks, float radius);
    std::vector<glm::vec4> generateNormals(std::size_t slices, std::size_t stacks);
};
}
}
