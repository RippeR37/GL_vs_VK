#include <base/Random.h>
#include <tests/test1/gl/SimpleBallsSceneTest.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/vec4.hpp>

#include <cmath>

namespace tests {
namespace test_gl {
SimpleBallsSceneTest::SimpleBallsSceneTest() : GLTest("SimpleBallsSceneTest")
{
}

void SimpleBallsSceneTest::setup()
{
    GLTest::setup();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    initProgram();
    initVBO();
    initVAO();
    initTestData();

    window_.setDisplayingFPS(true);
    window_.setFPSRefreshRate(1.0);
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

        updateBalls();

        vao_.unbind();
        program_.unbind();

        window_.update();
    }
}

void SimpleBallsSceneTest::teardown()
{
    GLTest::teardown();
}

void SimpleBallsSceneTest::initProgram()
{
    program_.load({"resources/test1/shaders/shader.vp", base::gl::Shader::Type::VertexShader},
                  {"resources/test1/shaders/shader.fp", base::gl::Shader::Type::FragmentShader});
}

/*
std::vector<glm::vec4> initSphereVertices2()
{
    const double PI = 3.141592653589793;

    double phiStep = 2 * PI / 5;
    double tettaStep = PI / 5;
    double mR = 1.0;
    std::vector<glm::vec4> vertices;

    for (double tetta = 0; tetta <= (PI - tettaStep) + tettaStep / 2; tetta += tettaStep) {
        for (double phi = 0; phi <= 2 * PI + phiStep / 2; phi += phiStep) {
            auto x1 = (float)(mR * std::sin(tetta) * std::cos(phi));
            auto y1 = (float)(mR * std::cos(tetta));
            auto z1 = (float)(mR * std::sin(tetta) * std::sin(phi));
            vertices.push_back({ x1, y1, z1, 1.0 });

            auto x2 = (float)(mR * std::sin(tetta + tettaStep) * std::cos(phi));
            auto y2 = (float)(mR * std::cos(tetta + tettaStep));
            auto z2 = (float)(mR * std::sin(tetta + tettaStep) * std::sin(phi));
            vertices.push_back({ x2, y2, z2, 1.0 });

            auto x3 = (float)(mR * std::sin(tetta) * std::cos(phi + phiStep));
            auto y3 = (float)(mR * std::cos(tetta));
            auto z3 = (float)(mR * std::sin(tetta) * std::sin(phi + phiStep));
            vertices.push_back({ x3, y3, z3, 1.0 });
        }
    }

    return vertices;
}
*/

std::vector<glm::vec4> initSphereVertices()
{
    int rings = 4;
    int sectors = 4;
    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);
    int r, s;

    const double PI = 3.141592653589793;
    const double PI_2 = 0.5 * PI;

    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> result;
    vertices.resize(rings * sectors);

    std::vector<glm::vec4>::iterator v = vertices.begin();

    for (r = 0; r < rings; r++)
        for (s = 0; s < sectors; s++) {
            float const y = (float)std::sin(-PI_2 + PI * r * R);
            float const x = (float)(std::cos(2 * PI * s * S) * std::sin(PI * r * R));
            float const z = (float)(std::sin(2 * PI * s * S) * std::sin(PI * r * R));

            *v++ = glm::vec4{x, y, z, 1.0};
        }

    result.resize(rings * sectors * 6);
    std::vector<glm::vec4>::iterator i = result.begin();

    for (r = 0; r < rings; r++) {
        for (s = 0; s < sectors; s++) {
            auto i1_1 = r * sectors + s;
            auto i1_2 = r * sectors + (s + 1);
            auto i1_3 = (r + 1) * sectors + (s + 1);
            *i++ = vertices[i1_1];
            *i++ = vertices[i1_2];
            *i++ = vertices[i1_3];

            auto i2_1 = r * sectors + s;
            auto i2_2 = (r + 1) * sectors + (s + 1);
            auto i2_3 = (r + 1) * sectors + s;
            *i++ = vertices[i2_1];
            *i++ = vertices[i2_2];
            *i++ = vertices[i2_3];
        }
    }

    return result;
}

void SimpleBallsSceneTest::initVBO()
{
    // Vertices
    if (vertices_.empty()) {
        vertices_ = {{-0.03f, -0.03f, 0.0f, 1.0f}, {0.03f, -0.03f, 0.0f, 1.0f}, {0.0f, 0.03f, 0.0f, 1.0f}};
    }

    vertices_ = initSphereVertices();

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

void SimpleBallsSceneTest::initTestData()
{
    static const size_t N = 1;
    static const float SPEED_SCALE = 0.3f;

    balls_.clear();
    balls_.reserve(N);

    for (int i = 0; i < N; ++i) {
        auto position = getRandomVec4({-1.0, -1.0, -1.0, 1.0}, {1.0, 1.0, 1.0, 1.0});
        auto color = getRandomVec4({0.0, 0.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0});
        auto speed = getRandomVec4({-1.0, -1.0, -1.0, 0.0}, {1.0, 1.0, 1.0, 0.0});

        balls_.push_back({position, color, (glm::normalize(speed) * SPEED_SCALE)});
    }
}

void SimpleBallsSceneTest::updateBalls()
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
