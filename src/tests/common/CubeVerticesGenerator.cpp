#include <tests/common/CubeVerticesGenerator.h>

#include <cmath>

namespace tests {
namespace common {
CubeVerticesGenerator::CubeVerticesGenerator()
    : vertices(generateVertices())
    , normals(generateNormals())
{
}

// Generates unit cube (size = 1.0) with center in (0, 0, 0)
std::vector<glm::vec4> CubeVerticesGenerator::generateVertices()
{
    std::vector<glm::vec4> result;

    result.push_back({-0.5f, -0.5f, -0.5f, 1.0f});
    result.push_back({-0.5f, -0.5f, 0.5f, 1.0f});
    result.push_back({-0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({0.5f, 0.5f, -0.5f, 1.0f});
    result.push_back({-0.5f, -0.5f, -0.5f, 1.0f});
    result.push_back({-0.5f, 0.5f, -0.5f, 1.0f});

    result.push_back({0.5f, -0.5f, 0.5f, 1.0f});
    result.push_back({-0.5f, -0.5f, -0.5f, 1.0f});
    result.push_back({0.5f, -0.5f, -0.5f, 1.0f});
    result.push_back({0.5f, 0.5f, -0.5f, 1.0f});
    result.push_back({0.5f, -0.5f, -0.5f, 1.0f});
    result.push_back({-0.5f, -0.5f, -0.5f, 1.0f});

    result.push_back({-0.5f, -0.5f, -0.5f, 1.0f});
    result.push_back({-0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({-0.5f, 0.5f, -0.5f, 1.0f});
    result.push_back({0.5f, -0.5f, 0.5f, 1.0f});
    result.push_back({-0.5f, -0.5f, 0.5f, 1.0f});
    result.push_back({-0.5f, -0.5f, -0.5f, 1.0f});

    result.push_back({-0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({-0.5f, -0.5f, 0.5f, 1.0f});
    result.push_back({0.5f, -0.5f, 0.5f, 1.0f});
    result.push_back({0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({0.5f, -0.5f, -0.5f, 1.0f});
    result.push_back({0.5f, 0.5f, -0.5f, 1.0f});

    result.push_back({0.5f, -0.5f, -0.5f, 1.0f});
    result.push_back({0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({0.5f, -0.5f, 0.5f, 1.0f});
    result.push_back({0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({0.5f, 0.5f, -0.5f, 1.0f});
    result.push_back({-0.5f, 0.5f, -0.5f, 1.0f});

    result.push_back({0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({-0.5f, 0.5f, -0.5f, 1.0f});
    result.push_back({-0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({-0.5f, 0.5f, 0.5f, 1.0f});
    result.push_back({0.5f, -0.5f, 0.5f, 1.0f});

    return result;
}

std::vector<glm::vec4> CubeVerticesGenerator::generateNormals()
{
    return std::vector<glm::vec4>(36);
}
}
}
