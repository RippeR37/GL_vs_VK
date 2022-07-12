#include <tests/test1/gl/SimpleBallsSceneTest.h>

#include <GL/glew.h>
#include <glm/vec4.hpp>

namespace tests {
namespace test_gl {
SimpleBallsSceneTest::SimpleBallsSceneTest(bool benchmarkMode, float benchmarkTime)
    : BaseBallsSceneTest()
    , GLTest("SimpleBallsSceneTest", benchmarkMode, benchmarkTime)
{
}

void SimpleBallsSceneTest::setup()
{
    GLTest::setup();
    initTestState();

    initApplication();
    initProgram();
    initVBO();
    initVAO();
}

void SimpleBallsSceneTest::run()
{
    while (!window_.shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);

        updateTestState(static_cast<float>(window_.getFrameTime()));

        program_.use();
        vao_.bind();

        for (const auto& ball : balls()) {
            program_["objPosition"] = ball.position;
            program_["objColor"] = ball.color;

            vao_.drawArrays();
        }

        vao_.unbind();
        program_.unbind();

        window_.update();

        if (processFrameTime(window_.getFrameTime())) {
            break; // Benchmarking is complete
        }
    }
}

void SimpleBallsSceneTest::teardown()
{
    destroyTestState();
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
    // VertexData
    base::gl::VertexBuffer::Data vertexData;
    vertexData.data = (GLvoid*)(vertices().data());
    vertexData.size = sizeof(glm::vec4) * vertices().size();
    vertexData.pointers.push_back(base::gl::VertexAttrib(0, 4, GL_FLOAT, 0, nullptr));

    // VBO settings
    vbo_.bind();
    vbo_.setData(vertexData);
    vbo_.unbind();
}

void SimpleBallsSceneTest::initVAO()
{
    vao_.setDrawCount(static_cast<GLsizei>(vertices().size()));
    vao_.setDrawTarget(base::gl::VertexArray::DrawTarget::Triangles);

    vao_.bind();
    vao_.attachVBO(&vbo_);
    vao_.setAttribPointers();
    vao_.unbind();
}
}
}
