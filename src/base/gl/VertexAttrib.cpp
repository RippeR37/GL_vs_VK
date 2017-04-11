#include <base/gl/VertexAttrib.h>

namespace base {
namespace gl {
VertexAttrib::VertexAttrib()
    : VertexAttrib(0, 0, GL_FLOAT, 0, nullptr)
{
    normalized = GL_FALSE;
}

VertexAttrib::VertexAttrib(GLuint index_, GLint size_, GLenum type_, GLsizei stride_, GLvoid* offset_)
{
    index = index_;
    size = size_;
    type = type_;
    normalized = GL_FALSE;
    stride = stride_;
    offset = offset_;
}

VertexAttrib::VertexAttrib(GLuint index_, GLint size_, GLenum type_, GLsizei stride_, std::size_t offset_)
    : VertexAttrib(index_, size_, type_, stride_, reinterpret_cast<GLvoid*>(offset_))
{
}

VertexAttrib::~VertexAttrib()
{
}

void VertexAttrib::set()
{
    glVertexAttribPointer(index, size, type, normalized, stride, offset);
}
}
}
