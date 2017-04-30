#pragma once

#include <base/gl/Program.h>
#include <base/gl/VertexArray.h>
#include <base/gl/VertexBuffer.h>
#include <framework/GLTest.h>
#include <tests/common/Ball.h>
#include <tests/test1/BaseBallsSceneTest.h>

namespace tests {
namespace test_gl {
class MultithreadedBallsSceneTest : public BaseBallsSceneTest, public framework::GLTest
{
  public:
    MultithreadedBallsSceneTest();

    void setup() override;
    void run() override;
    void teardown() override;

  private:
    void initApplication();
    void initProgram();
    void initVBO();
    void initVAO();

    void updateStateMultithreaded();
    void updatePartialState(std::size_t rangeFrom, std::size_t rangeTo);

    base::gl::Program program_;
    base::gl::VertexArray vao_;
    base::gl::VertexBuffer vbo_;
};
}
}
