#include <base/Random.h>
#include <tests/test1/gl/SimpleBallsSceneTest.h>

#include <GL/glew.h>
#include <glm/vec4.hpp>

namespace tests {
namespace gl {
    SimpleBallsSceneTest::SimpleBallsSceneTest()
        : GLTest("SimpleBallsSceneTest")
    {
    }

    void SimpleBallsSceneTest::setup()
    {
        GLTest::setup();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        initProgram();
        initVBO();
        initVAO();
        initTestData();
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
        program_.load(
            { "resources/test1/shaders/shader.vp", base::gl::Shader::Type::VertexShader },
            { "resources/test1/shaders/shader.fp", base::gl::Shader::Type::FragmentShader });
    }

    void SimpleBallsSceneTest::initVBO()
    {
        // Vertices
        if (vertices_.empty()) {
            vertices_ = {
                { -0.03f, -0.03f, 0.0f, 1.0f },
                { 0.03f, -0.03f, 0.0f, 1.0f },
                { 0.0f, 0.03f, 0.0f, 1.0f }
            };
        }

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
        static const size_t N = 1000;

        balls_.clear();
        balls_.reserve(N);

        for (int i = 0; i < N; ++i) {
            auto position = getRandomVec4({ -1.0, -1.0, -1.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 });
            auto color = getRandomVec4({ 0.0, 0.0, 0.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 });
            auto speed = getRandomVec4({ -1.0, -1.0, -1.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 });

            balls_.push_back({ position, color, speed });
        }
    }
}
}
