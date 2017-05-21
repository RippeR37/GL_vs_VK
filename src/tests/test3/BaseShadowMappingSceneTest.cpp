#include <tests/test3/BaseShadowMappingSceneTest.h>

#include <tests/common/CubeVerticesGenerator.h>
#include <tests/common/SphereVerticesGenerator.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace {
const float kFoV = 45.0f;
const float kAspectRatio = 800.0f / 600.0f;
const float kUpdateTimeFactor = 0.5f;
const glm::vec3 kCameraPosition = {0.0f, 5.0, -15.0};
const glm::vec3 kCameraTarget = {0.0f, 0.0f, 0.0f};
const glm::vec2 kClipDistances = {1.0f, 2000.0f};
const glm::vec3 kLightDirection = {1.0f, -0.5f, 0.5f};
const glm::vec3 kLightPosition = glm::normalize(-kLightDirection) * 5.0f;
const glm::uvec2 kShadowmapSize = { 4096, 4096 };

std::vector<glm::vec4> generateColorBuffer(std::size_t count, const glm::vec4& color)
{
    return std::vector<glm::vec4>(count, color);
}
}

namespace tests {
BaseShadowMappingSceneTest::BaseShadowMappingSceneTest()
    : _projectionMatrix(1.0f)
    , _viewMatrix(1.0f)
{
    initMatrices();
    createRenderObjects();
}

void BaseShadowMappingSceneTest::updateTestState(double /*dt*/)
{
    double time = glfwGetTime() * kUpdateTimeFactor;
    _viewMatrix = glm::lookAt(glm::vec3{std::cos(time) * 10.0, 5.0, std::sin(time) * 10.0}, //
                              kCameraTarget, //
                              glm::vec3{0.0f, 1.0f, 0.0f});
}

const std::vector<common::RenderObject>& BaseShadowMappingSceneTest::renderObjects() const
{
    return _renderObjects;
}

const glm::mat4& BaseShadowMappingSceneTest::renderMatrix() const
{
    return _projectionMatrix * _viewMatrix;
}

const glm::mat4& BaseShadowMappingSceneTest::shadowMatrix() const
{
    return _shadowMatrix;
}

const glm::uvec2& BaseShadowMappingSceneTest::shadowmapSize() const
{
    return kShadowmapSize;
}

void BaseShadowMappingSceneTest::initMatrices()
{
    _projectionMatrix = glm::perspective(kFoV, kAspectRatio, kClipDistances[0], kClipDistances[1]);
    _viewMatrix = glm::lookAt(kCameraPosition, kCameraTarget, glm::vec3{0.0f, 1.0f, 0.0f});

    glm::mat4 shadowProjectionMatrix = glm::ortho(-30.0f, 30.0f, -15.0f, 15.0f, -30.0f, 30.0f);
    glm::mat4 shadowViewMatrix = glm::lookAt(kLightPosition, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
    _shadowMatrix = shadowProjectionMatrix * shadowViewMatrix;
}

void BaseShadowMappingSceneTest::createRenderObjects()
{
    // Bottom
    for (int x = -10; x < 10; ++x) {
        for (int y = -10; y < 10; ++y) {
            common::CubeVerticesGenerator cubeGenerator;
            glm::mat4 cubeModelMatrix(1.0f);
            glm::vec4 cubeColor = {0.4f, 0.4f, 0.4f, 1.0f};
            if ((x + y) % 2) {
                cubeColor += glm::vec4{0.1f, 0.1f, 0.1f, 0.0f};
            }
            cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3{5.0f, 1.0f, 5.0f});
            cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3{x, -0.5f, y});

            _renderObjects.push_back({cubeModelMatrix, cubeGenerator.vertices, cubeGenerator.normals,
                                      generateColorBuffer(cubeGenerator.vertices.size(), cubeColor)});
        }
    }

    // Sphere #1
    {
        common::SphereVerticesGenerator sphereGenerator{30, 30, 1.0f};
        glm::mat4 sphereModelMatrix(1.0f);
        glm::vec4 sphereColor = {0.2f, 0.2f, 0.2f, 1.0f};
        sphereModelMatrix = glm::translate(sphereModelMatrix, glm::vec3{0.0f, 5.0f, 0.0f});
        sphereModelMatrix = glm::scale(sphereModelMatrix, glm::vec3{2.0f, 2.0f, 2.0f});

        _renderObjects.push_back({sphereModelMatrix, sphereGenerator.vertices, sphereGenerator.normals,
                                  generateColorBuffer(sphereGenerator.vertices.size(), sphereColor)});
    }

    // Boxes
    for (int x = -10; x < 10; ++x) {
        for (int y = -10; y < 10; ++y) {
            common::CubeVerticesGenerator cubeGenerator;
            glm::mat4 cubeModelMatrix(1.0f);
            glm::vec4 cubeColor = {0.25f, 0.25f, 0.25f, 1.0f};
            cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3{2.0f * x, 1.0f, 2.0f * y});
            if ((x + y) % 2) {
                cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3{0.0f, 1.0f, 0.0f});
            }

            _renderObjects.push_back({cubeModelMatrix, cubeGenerator.vertices, cubeGenerator.normals,
                                      generateColorBuffer(cubeGenerator.vertices.size(), cubeColor)});
        }
    }
}
}
