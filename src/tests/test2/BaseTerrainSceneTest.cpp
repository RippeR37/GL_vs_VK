#include <tests/test2/BaseTerrainSceneTest.h>

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

namespace {
const char kHeightmapPath[] = "resources/test2/heightmaps/heightmap_1024x1024.raw";
const std::size_t kHeightmapWidth = 1024;
const std::size_t kHeightmapHeight = 1024;
const float kUpdateTimeFactor = 0.5f;
const float kUpdateRadius = 384.0f;
const glm::vec2 kUpdateCenter = {512.0f, 512.0f};
const float kLoDFactor = 64.0f;
const glm::vec3 kCameraPosition = {-100.0f, 300.0, -100.0};
const glm::vec3 kCameraTarget = {256.0f, 0.0f, 256.0f};
const float kFoV = 45.0f;
const float kAspectRation = 800.0f / 600.0f;
const glm::vec2 kClipDistances = {1.0f, 2000.0f};
const glm::mat4 kModelMatrix{1.0f};
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

const glm::mat4& BaseTerrainSceneTest::currentMVP() const
{
    static const glm::mat4 projection = glm::perspective(kFoV, kAspectRation, kClipDistances[0], kClipDistances[1]);
    static const glm::mat4 view = glm::lookAt(kCameraPosition, kCameraTarget, glm::vec3{0.0f, 1.0f, 0.0f});
    static const glm::mat4 MVP = projection * view * kModelMatrix;

    return MVP;
}
}
