#pragma once

#include <base/gl/Program.h>
#include <base/gl/VertexArray.h>
#include <base/gl/VertexBuffer.h>
#include <framework/GLTest.h>
#include <tests/common/Ball.h>

namespace tests {
namespace test_gl {
class SimpleBallsSceneTest : public framework::GLTest
{
  public:
    SimpleBallsSceneTest();

    void setup() override;
    void run() override;
    void teardown() override;

  private:
    void initProgram();
    void initVBO();
    void initVAO();
    void initTestData();

    void updateBalls();

    base::gl::Program program_;
    base::gl::VertexArray vao_;
    base::gl::VertexBuffer vbo_;
    std::vector<glm::vec4> vertices_;
    std::vector<common::Ball> balls_;
};
}
}
