#pragma once

#include <base/gl/Program.h>
#include <base/gl/VertexArray.h>
#include <base/gl/VertexBuffer.h>
#include <framework/GLTest.h>
#include <tests/test3/BaseShadowMappingSceneTest.h>

namespace tests {
namespace test_gl {
class ShadowMappingSceneTest : public BaseShadowMappingSceneTest, public framework::GLTest
{
  public:
    ShadowMappingSceneTest(bool benchmarkMode, float benchmarkTime);

    void setup() override;
    void run() override;
    void teardown() override;

  private:
    struct GLRenderObject
    {
        GLRenderObject() = default;
        GLRenderObject(GLRenderObject&& other)
            : modelMatrix(std::move(other.modelMatrix))
            , vao(std::move(other.vao))
            , vbo(std::move(other.vbo))
        {
        }

        GLRenderObject(const GLRenderObject&) = delete;
        GLRenderObject& operator=(const GLRenderObject&) = delete;

        glm::mat4 modelMatrix;
        base::gl::VertexArray vao;
        base::gl::VertexBuffer vbo;
    };

    void initApplication();
    void initShadowmapObjects();
    void initPrograms();
    void initRenderObjects();
    void initVBO(const common::RenderObject& renderObject, GLRenderObject& glRenderObject);
    void initVAO(const common::RenderObject& renderObject, GLRenderObject& glRenderObject);

    void setupShadowStage();
    void setupRenderStage();
    void render(const base::gl::Program& currentProgram, const glm::mat4& viewProjectionmatrix);

    glm::mat4 convertProjectionToImage(const glm::mat4& matrix) const;

    GLuint _shadowmapFramebuffer;
    GLuint _shadowmapTexture;
    base::gl::Program _shadowProgram;
    base::gl::Program _renderProgram;
    std::vector<GLRenderObject> _glRenderObjects;
};
}
}
