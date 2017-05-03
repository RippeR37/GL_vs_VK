#pragma once

#include <glm/vec2.hpp>

#include <array>
#include <memory>

namespace tests {
namespace common {
struct QTNode
{
    glm::vec2 position;
    glm::vec2 size;
    std::size_t indexCount;
    std::size_t indexOffset;
    std::array<std::unique_ptr<QTNode>, 4> nodes;
};
}
}
