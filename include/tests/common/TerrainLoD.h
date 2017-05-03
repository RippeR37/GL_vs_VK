#pragma once

#include <tests/common/Heightmap.h>
#include <tests/common/QTNode.h>

#include <glm/vec4.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace tests {
namespace common {
class TerrainLoD
{
  public:
    TerrainLoD(const Heightmap& heightmap, float lodFactor);

    const std::vector<glm::vec4>& vertices() const;
    const std::vector<uint32_t>& indices() const;

    void executeLoD(const glm::vec2& position, const std::function<void(std::size_t, std::ptrdiff_t)>& function) const;

  private:
    void load(const Heightmap& heightmap);
    void computeNodesRecursive(const Heightmap& heightmap,
                               QTNode& node,
                               const glm::uvec2& offset,
                               const glm::uvec2& size);
    void executeLoDRecursive(const glm::vec2& position,
                             const std::function<void(std::size_t, std::ptrdiff_t)>& function,
                             const QTNode& node) const;

    const float _lodFactor;
    QTNode _root;
    std::vector<glm::vec4> _vertices;
    std::vector<uint32_t> _indices;
};
}
}
