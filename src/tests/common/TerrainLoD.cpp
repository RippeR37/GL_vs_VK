#include <tests/common/TerrainLoD.h>

#include <glm/geometric.hpp>
#include <glm/gtx/component_wise.hpp>

namespace {
uint32_t indexOf(const tests::common::Heightmap& heightmap, std::size_t x, std::size_t y)
{
    return static_cast<uint32_t>(heightmap.getSize().x * y + x);
}

std::size_t pushQuadIndices(const tests::common::Heightmap& heightmap,
                            std::vector<uint32_t>& indices,
                            const glm::uvec2& size,
                            const glm::uvec2& offset)
{
    std::size_t x1 = offset.x;
    std::size_t y1 = offset.y;
    std::size_t x2 = x1 + size.x;
    std::size_t y2 = y1 + size.y;

    indices.push_back(indexOf(heightmap, x1, y1));
    indices.push_back(indexOf(heightmap, x2, y1));
    indices.push_back(indexOf(heightmap, x1, y2));

    indices.push_back(indexOf(heightmap, x1, y2));
    indices.push_back(indexOf(heightmap, x2, y1));
    indices.push_back(indexOf(heightmap, x2, y2));

    return 6;
}
}

namespace tests {
namespace common {
TerrainLoD::TerrainLoD(const Heightmap& heightmap, float lodFactor)
    : _lodFactor(lodFactor)
{
    load(heightmap);
}

const std::vector<glm::vec4>& TerrainLoD::vertices() const
{
    return _vertices;
}

const std::vector<uint32_t>& TerrainLoD::indices() const
{
    return _indices;
}

void TerrainLoD::executeLoD(const glm::vec2& position,
                            const std::function<void(std::size_t, std::ptrdiff_t)>& function) const
{
    executeLoDRecursive(position, function, _root);
}

void TerrainLoD::executeLoD(const glm::vec2& position,
                            const std::function<void(std::size_t, std::ptrdiff_t)>& function,
                            std::size_t nodeIndex) const
{
    switch (nodeIndex) {
    case 0:
    case 1:
    case 2:
    case 3:
        if (_root.nodes[nodeIndex]) {
            executeLoDRecursive(position, function, *_root.nodes[nodeIndex]);
        }
        break;
    default:
        break;
    }
}

void TerrainLoD::load(const Heightmap& heightmap)
{
    glm::uvec2 mapSize = heightmap.getSize();
    glm::vec4 mapScale = {1.0, 4.0f, 1.0, 1.0f};

    // Vertices
    _vertices.reserve(heightmap.getData().size());
    for (std::size_t z = 0; z < mapSize.y; ++z) {
        for (std::size_t x = 0; x < mapSize.x; ++x) {
            glm::vec4 vertex = {x, heightmap.getHeight(x, z), z, 1.0};
            _vertices.push_back(vertex / mapScale);
        }
    }

    // Indices - recursive for each level and all sub-levels
    computeNodesRecursive(heightmap, _root, {0, 0}, mapSize - glm::uvec2{1, 1});
}

void TerrainLoD::computeNodesRecursive(const Heightmap& heightmap,
                                       QTNode& node,
                                       const glm::uvec2& offset,
                                       const glm::uvec2& size)
{
    // Compute this node
    node.size = size;
    node.position = offset;
    node.indexOffset = _indices.size() * sizeof(_indices.front());
    node.indexCount = pushQuadIndices(heightmap, _indices, size, offset);

    // Child nodes
    bool isLeaf = ((size.x <= 1) || (size.y <= 1));
    if (!isLeaf) {
        for (auto& child : node.nodes) {
            child.reset(new QTNode());
        }

        glm::uvec2 halfSize = {size.x / 2, size.y / 2};
        computeNodesRecursive(heightmap, *node.nodes[0], {offset.x, offset.y}, {halfSize.x, halfSize.y});
        computeNodesRecursive(heightmap, *node.nodes[1], {offset.x + halfSize.x, offset.y},
                              {size.x - halfSize.x, halfSize.y});
        computeNodesRecursive(heightmap, *node.nodes[2], {offset.x, offset.y + halfSize.y},
                              {halfSize.x, size.y - halfSize.y});
        computeNodesRecursive(heightmap, *node.nodes[3], {offset + halfSize}, {size - halfSize});
    }
}

void TerrainLoD::executeLoDRecursive(const glm::vec2& position,
                                     const std::function<void(std::size_t, std::ptrdiff_t)>& function,
                                     const QTNode& node) const
{
    if (!node.nodes.front()) {
        function(node.indexCount, node.indexOffset);

    } else {
        float distance = glm::distance(position, node.position + (node.size / 2.0f));
        float maxSize = glm::compMax(node.size);

        if (distance / maxSize >= _lodFactor) {
            function(node.indexCount, node.indexOffset);

        } else {
            for (const auto& subnode : node.nodes) {
                executeLoDRecursive(position, function, *subnode);
            }
        }
    }
}
}
}
