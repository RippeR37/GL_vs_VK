#pragma once

#include <base/gl/VertexAttrib.h>
#include <base/gl/VertexBuffer.h>

#include <GL/glew.h>

#include <vector>

namespace base {
namespace gl {

    class VertexArray {
    public:
        enum class DrawTarget : GLenum {
            Points = GL_POINTS,
            LineStrip = GL_LINE_STRIP,
            LineLoop = GL_LINE_LOOP,
            Lines = GL_LINES,
            LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
            LinesAdjacency = GL_LINES_ADJACENCY,
            TriangleStrip = GL_TRIANGLE_STRIP,
            TriangleFan = GL_TRIANGLE_FAN,
            Triangles = GL_TRIANGLES,
            TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
            TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,
            Patches = GL_PATCHES
        };

    public:
        VertexArray();
        VertexArray(VertexArray&& vao);
        VertexArray(const VertexArray&) = delete;
        ~VertexArray();

        VertexArray& operator=(const VertexArray&) = delete;
        VertexArray& operator=(VertexArray&& vao);

        void create();
        void destroy();

        void bind() const;
        void unbind() const;
        void drawArrays() const;

        void enableAttrib(GLuint index);
        void disableAttrib(GLuint index);

        void attachVBO(const VertexBuffer* vertexBuffer);

        void setAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);

        void setAttribPointers();
        void setAttribPointers(const VertexBuffer& vertexBuffer);
        void setAttribPointers(const std::vector<VertexAttrib>& attributes);

        void setDrawOffset(GLint offset);
        void setDrawCount(GLsizei count);
        void setDrawTarget(DrawTarget target);

        GLint getDrawOffset() const;
        GLsizei getDrawCount() const;
        DrawTarget getDrawTarget() const;
        GLuint getID() const;

        const std::list<const VertexBuffer*>& getAttachedVBOs() const;

    private:
        bool isCreated() const;

        bool _isCreated;
        bool _isDrawTargetSet;
        bool _isDrawCountSet;

        GLuint _vaoID;
        GLint _drawOffset;
        GLsizei _drawCount;
        DrawTarget _drawTarget;
        std::list<const VertexBuffer*> _attachedVBOs;
    };
}
}
