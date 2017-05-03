#include <tests/test2/BaseTerrainSceneTest.h>

#include <GLFW/glfw3.h>

namespace {
const char kHeightmapPath[] = "resources/test2/heightmaps/heightmap_1024x1024.raw";
const std::size_t kHeightmapWidth = 1024;
const std::size_t kHeightmapHeight = 1024;
const float kUpdateTimeFactor = 1.0f;
const float kUpdateRadius = 384.0f;
const glm::vec2 kUpdateCenter = {512.0f, 512.0f};
const float kLoDFactor = 64.0f;
}

namespace tests {
BaseTerrainSceneTest::BaseTerrainSceneTest()
    : _terrain({kHeightmapPath, kHeightmapWidth, kHeightmapHeight}, kLoDFactor)
{
}

void BaseTerrainSceneTest::updateTestState(double /*dt*/)
{
    float time = kUpdateTimeFactor * static_cast<float>(glfwGetTime());
    _position = kUpdateCenter + glm::vec2{std::cos(time), std::sin(time)} * kUpdateRadius;
}

const common::TerrainLoD& BaseTerrainSceneTest::terrain() const
{
    return _terrain;
}

const glm::vec2& BaseTerrainSceneTest::currentPosition() const
{
    return _position;
}
}
