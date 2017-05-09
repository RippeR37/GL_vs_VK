#include <tests/test3/BaseShadowMappingSceneTest.h>

#include <tests/common/CubeVerticesGenerator.h>
#include <tests/common/SphereVerticesGenerator.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace {
const float kFoV = 45.0f;
const float kAspectRatio = 800.0f / 600.0f;
const glm::vec3 kCameraPosition = {0.0f, 5.0, -5.0};
const glm::vec3 kCameraTarget = {0.0f, 0.0f, 0.0f};
const glm::vec2 kClipDistances = {1.0f, 2000.0f};
const glm::vec3 kLightDirection = {1.0f, -0.5f, 0.5f};
const glm::vec3 kLightPosition = glm::normalize(-kLightDirection) * 5.0f;

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
    double time = glfwGetTime();
    _viewMatrix = glm::lookAt(glm::vec3{std::cos(time) * 5.0, 5.0, std::sin(time) * 5.0}, //
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

glm::mat4 BaseShadowMappingSceneTest::applyDepthBias(const glm::mat4& matrix) const
{
    static const glm::mat4 bias = {0.5, 0.0, 0.0, 0.0, //
                                   0.0, 0.5, 0.0, 0.0, //
                                   0.0, 0.0, 0.5, 0.0, //
                                   0.5, 0.5, 0.5, 1.0};

    return bias * matrix;
}

void BaseShadowMappingSceneTest::initMatrices()
{
    _projectionMatrix = glm::perspective(kFoV, kAspectRatio, kClipDistances[0], kClipDistances[1]);
    _viewMatrix = glm::lookAt(kCameraPosition, kCameraTarget, glm::vec3{0.0f, 1.0f, 0.0f});

    glm::mat4 shadowProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
    glm::mat4 shadowViewMatrix = glm::lookAt(kLightPosition, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
    _shadowMatrix = shadowProjectionMatrix * shadowViewMatrix;
}

void BaseShadowMappingSceneTest::createRenderObjects()
{
    // TODO: add more objects!

    // Sphere #1
    {
        common::SphereVerticesGenerator sphereGenerator{30, 30, 1.0f};
        glm::mat4 sphereModelMatrix(1.0f);
        glm::vec4 sphereColor = {0.2f, 0.2f, 0.2f, 1.0f};
        sphereModelMatrix = glm::translate(sphereModelMatrix, glm::vec3{0.0f, 2.0f, 0.0f});

        _renderObjects.push_back({sphereModelMatrix, sphereGenerator.vertices, sphereGenerator.normals,
                                  generateColorBuffer(sphereGenerator.vertices.size(), sphereColor)});
    }

    // Cube #1
    {
        common::CubeVerticesGenerator cubeGenerator;
        glm::mat4 cubeModelMatrix(1.0f);
        glm::vec4 cubeColor = {0.3f, 0.3f, 0.3f, 1.0f};
        cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3{2.0f, 2.0f, 0.0f});

        _renderObjects.push_back({cubeModelMatrix, cubeGenerator.vertices, cubeGenerator.normals,
                                  generateColorBuffer(cubeGenerator.vertices.size(), cubeColor)});
    }

    // Cube #2
    {
        common::CubeVerticesGenerator cubeGenerator;
        glm::mat4 cubeModelMatrix(1.0f);
        glm::vec4 cubeColor = {0.3f, 0.3f, 0.3f, 1.0f};
        cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3{-2.0f, 0.5f, 0.0f});

        _renderObjects.push_back({cubeModelMatrix, cubeGenerator.vertices, cubeGenerator.normals,
                                  generateColorBuffer(cubeGenerator.vertices.size(), cubeColor)});
    }

    // Bottom
    {
        common::CubeVerticesGenerator cubeGenerator;
        glm::mat4 cubeModelMatrix(1.0f);
        glm::vec4 cubeColor = {0.4f, 0.4f, 0.4f, 1.0f};
        cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3{14.0f, 0.5f, 10.0f});

        _renderObjects.push_back({cubeModelMatrix, cubeGenerator.vertices, cubeGenerator.normals,
                                  generateColorBuffer(cubeGenerator.vertices.size(), cubeColor)});
    }
}
}
