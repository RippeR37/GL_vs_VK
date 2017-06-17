#pragma once

#include <base/gl/Program.h>
#include <base/gl/VertexArray.h>
#include <base/gl/VertexBuffer.h>
#include <framework/GLTest.h>
#include <tests/test4/BaseInitializationTest.h>

namespace tests {
namespace test_gl {
class InitializationTest : public BaseInitializationTest, public framework::GLTest
{
  public:
    InitializationTest();

    void setup() override;
    void run() override;
    void teardown() override;

  private:
    void initApplication();
    void initProgram();
    void initVBO();
    void initVAO();

    base::gl::Program program_;
    base::gl::VertexArray vao_;
    base::gl::VertexBuffer vbo_;
};
}
}
