#pragma once

#include <glm/matrix.hpp>

#include <vector>

namespace tests {
namespace common {
struct RenderObject
{
    std::vector<glm::vec4> generateCombinedData() const
    {
        std::vector<glm::vec4> result;

        for (std::size_t index = 0; index < vertices.size(); ++index) {
            result.push_back(vertices[index]);
            result.push_back(normals[index]);
            result.push_back(colors[index]);
        }

        return result;
    }

    glm::mat4 modelMatrix;
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> colors;
};
}
}
