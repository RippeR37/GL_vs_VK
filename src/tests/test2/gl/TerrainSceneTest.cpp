#include <tests/test2/gl/TerrainSceneTest.h>

#include <GL/glew.h>
#include <glm/vec4.hpp>

namespace tests {
namespace test_gl {
TerrainSceneTest::TerrainSceneTest(bool benchmarkMode, float benchmarkTime)
    : BaseTerrainSceneTest()
    , GLTest("TerrainSceneTest", benchmarkMode, benchmarkTime)
    , _ibo(base::gl::Buffer::Target::ElementArray, base::gl::Buffer::Usage::StaticDraw)
{
}

void TerrainSceneTest::setup()
{
    GLTest::setup();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    initApplication();
    initProgram();
    initVBO();
    initIBO();
    initVAO();
}

void TerrainSceneTest::run()
{
    while (!window_.shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);
        _program.use();
        _vao.bind();
        _ibo.bind(base::gl::Buffer::Target::ElementArray);

        _program["MVP"] = currentMVP();
        {
            auto renderChunk = [](std::size_t count, std::ptrdiff_t offset) {
                glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(count), GL_UNSIGNED_INT, (const GLvoid*)offset);
            };
            terrain().executeLoD(currentPosition(), renderChunk);
        }

        _ibo.unbind();
        _vao.unbind();
        _program.unbind();

        window_.update();
        updateTestState(window_.getFrameTime());

        if (processFrameTime(window_.getFrameTime())) {
            break; // Benchmarking is complete
        }
    }
}

void TerrainSceneTest::teardown()
{
    GLTest::teardown();
}

void TerrainSceneTest::initApplication()
{
    window_.setDisplayingFPS(true);
    window_.setFPSRefreshRate(1.0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void TerrainSceneTest::initProgram()
{
    _program.load({"resources/test2/shaders/gl_shader.vert", base::gl::Shader::Type::VertexShader},
                  {"resources/test2/shaders/gl_shader.frag", base::gl::Shader::Type::FragmentShader});
}

void TerrainSceneTest::initVBO()
{
    // VertexData
    base::gl::VertexBuffer::Data vertexData;
    vertexData.data = (GLvoid*)(terrain().vertices().data());
    vertexData.size = sizeof(glm::vec4) * terrain().vertices().size();
    vertexData.pointers.push_back(base::gl::VertexAttrib(0, 4, GL_FLOAT, 0, nullptr));

    // VBO settings
    _vbo.bind();
    _vbo.setData(vertexData);
    _vbo.unbind();
}

void TerrainSceneTest::initIBO()
{
    _ibo.bind(base::gl::Buffer::Target::ElementArray);
    _ibo.setData(terrain().indices());
    _ibo.unbind();
}

void TerrainSceneTest::initVAO()
{
    _vao.setDrawTarget(base::gl::VertexArray::DrawTarget::Triangles);

    _vao.bind();
    _vao.attachVBO(&_vbo);
    _vao.setAttribPointers();
    _vao.unbind();
}
}
}
