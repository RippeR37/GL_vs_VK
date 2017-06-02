#pragma once

#include <base/gl/Program.h>
#include <base/gl/VertexArray.h>
#include <base/gl/VertexBuffer.h>
#include <framework/GLTest.h>
#include <tests/test2/BaseTerrainSceneTest.h>

namespace tests {
namespace test_gl {
class TerrainSceneTest : public BaseTerrainSceneTest, public framework::GLTest
{
  public:
    TerrainSceneTest(bool benchmarkMode, float benchmarkTime);

    void setup() override;
    void run() override;
    void teardown() override;

  private:
    void initApplication();
    void initProgram();
    void initVBO();
    void initIBO();
    void initVAO();

    base::gl::Program _program;
    base::gl::VertexArray _vao;
    base::gl::VertexBuffer _vbo;
    base::gl::Buffer _ibo;
};
}
}
