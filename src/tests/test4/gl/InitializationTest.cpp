#include <tests/test4/gl/InitializationTest.h>

#include <GL/glew.h>
#include <glm/vec4.hpp>

namespace tests {
namespace test_gl {
InitializationTest::InitializationTest()
    : BaseInitializationTest()
    , GLTest("InitializationTest", true, 0.0f) // always benchmark mode, 1st frame only
{
}

void InitializationTest::setup()
{
    GLTest::setup();

    initApplication();
    initProgram();
    initVBO();
    initVAO();
}

void InitializationTest::run()
{
    glClear(GL_COLOR_BUFFER_BIT);

    program_.use();
    vao_.bind();
    vao_.drawArrays();
    vao_.unbind();
    program_.unbind();

    window_.update();

    // Synchronize CPU<->GPU and time it
    glFinish();
    processFrameTime();
}

void InitializationTest::teardown()
{
    GLTest::teardown();
}

void InitializationTest::initApplication()
{
    window_.setDisplayingFPS(true);
    window_.setFPSRefreshRate(1.0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void InitializationTest::initProgram()
{
    program_.load({"resources/test4/shaders/gl_shader.vert", base::gl::Shader::Type::VertexShader},
                  {"resources/test4/shaders/gl_shader.frag", base::gl::Shader::Type::FragmentShader});
}

void InitializationTest::initVBO()
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

void InitializationTest::initVAO()
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
