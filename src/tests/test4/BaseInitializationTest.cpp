#include <tests/test4/BaseInitializationTest.h>

namespace {
const std::vector<glm::vec4> kVertices{
    glm::vec4{-0.5f, -0.5f, 0.0f, 1.0f}, //
    glm::vec4{0.5f, -0.5f, 0.0f, 1.0f}, //
    glm::vec4{0.0f, 0.5f, 0.0f, 1.0f}, //
};
}

namespace tests {
const std::vector<glm::vec4>& BaseInitializationTest::vertices() const
{
    return kVertices;
}
}
