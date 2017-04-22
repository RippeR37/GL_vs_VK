#include <tests/test1/gl/SimpleBallsSceneTest.h>

#include <base/Random.h>
#include <tests/common/SphereVerticesGenerator.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/vec4.hpp>

#include <cmath>

namespace tests {
namespace test_gl {
SimpleBallsSceneTest::SimpleBallsSceneTest()
    : GLTest("SimpleBallsSceneTest")
{
}

void SimpleBallsSceneTest::setup()
{
    GLTest::setup();

    initApplication();
    initProgram();
    initVBO();
    initVAO();
    initState();
}

void SimpleBallsSceneTest::run()
{
    while (!window_.shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);

        program_.use();
        vao_.bind();

        for (const auto& ball : balls_) {
            program_["objPosition"] = ball.position;
            program_["objColor"] = ball.color;

            vao_.drawArrays();
        }

        updateState();

        vao_.unbind();
        program_.unbind();

        window_.update();
    }
}

void SimpleBallsSceneTest::teardown()
{
    GLTest::teardown();
}

void SimpleBallsSceneTest::initApplication()
{
    window_.setDisplayingFPS(true);
    window_.setFPSRefreshRate(1.0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void SimpleBallsSceneTest::initProgram()
{
    program_.load({"resources/test1/shaders/gl_shader.vert", base::gl::Shader::Type::VertexShader},
                  {"resources/test1/shaders/gl_shader.frag", base::gl::Shader::Type::FragmentShader});
}

void SimpleBallsSceneTest::initVBO()
{
    // Vertices
    // TODO: move to TC-common
    common::SphereVerticesGenerator verticesGenerator{4, 4};
    vertices_ = verticesGenerator.vertices;

    // VertexData
    base::gl::VertexBuffer::Data vertexData;
    vertexData.data = vertices_.data();
    vertexData.size = sizeof(glm::vec4) * vertices_.size();
    vertexData.pointers.push_back(base::gl::VertexAttrib(0, 4, GL_FLOAT, 0, nullptr));

    // VBO settings
    vbo_.bind();
    vbo_.setData(vertexData);
    vbo_.unbind();
}

void SimpleBallsSceneTest::initVAO()
{
    vao_.setDrawCount(vertices_.size());
    vao_.setDrawTarget(base::gl::VertexArray::DrawTarget::Triangles);

    vao_.bind();
    vao_.attachVBO(&vbo_);
    vao_.setAttribPointers();
    vao_.unbind();
}

glm::vec4 getRandomVec4(glm::vec4 min, glm::vec4 max)
{
    glm::vec4 result;

    result.x = base::random::getRandomRealFromRange(min.x, max.x);
    result.y = base::random::getRandomRealFromRange(min.y, max.y);
    result.z = base::random::getRandomRealFromRange(min.z, max.z);
    result.w = base::random::getRandomRealFromRange(min.w, max.w);

    return result;
}

// TODO: move to TC-common
void SimpleBallsSceneTest::initState()
{
    static const size_t N = 50000; // TODO: move to TC-attribute
    static const float SPEED_SCALE = 0.3f;

    balls_.clear();
    balls_.reserve(N);

    for (size_t i = 0; i < N; ++i) {
        auto position = getRandomVec4({-1.0, -1.0, -1.0, 0.0}, {1.0, 1.0, 1.0, 0.0});
        auto color = getRandomVec4({0.0, 0.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0});
        auto speed = getRandomVec4({-1.0, -1.0, -1.0, 0.0}, {1.0, 1.0, 1.0, 0.0});

        balls_.push_back({position, color, (glm::normalize(speed) * SPEED_SCALE)});
    }
}

// TODO: move to TC-common
void SimpleBallsSceneTest::updateState()
{
    float deltaTime = static_cast<float>(window_.getFrameTime());

    auto clampFloat = [](float& v, float min, float max) -> bool {
        if (v < min) {
            v = min;
            return true;
        } else if (v > max) {
            v = max;
            return true;
        }
        return false;
    };

    for (auto& ball : balls_) {
        ball.position += (deltaTime * ball.speed);

        if (clampFloat(ball.position.x, -1.0, 1.0))
            ball.speed.x *= (-1.0);
        if (clampFloat(ball.position.y, -1.0, 1.0))
            ball.speed.y *= (-1.0);
        if (clampFloat(ball.position.z, -1.0, 1.0))
            ball.speed.z *= (-1.0);
    }
}
}
}
