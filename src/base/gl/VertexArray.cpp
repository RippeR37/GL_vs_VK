#include <base/gl/VertexArray.h>

#include <iostream>
#include <utility>

namespace base {
namespace gl {
VertexArray::VertexArray()
{
    _isCreated = false;
    _isDrawTargetSet = false;
    _isDrawCountSet = false;
    _vaoID = 0;
    _drawOffset = 0;
    _drawCount = 0;
    _drawTarget = DrawTarget::Triangles;
}

VertexArray::VertexArray(VertexArray&& vao)
    : VertexArray()
{
    std::swap(_vaoID, vao._vaoID);
    std::swap(_isCreated, vao._isCreated);
    std::swap(_isDrawTargetSet, vao._isDrawTargetSet);
    std::swap(_isDrawCountSet, vao._isDrawCountSet);
    std::swap(_drawOffset, vao._drawOffset);
    std::swap(_drawTarget, vao._drawTarget);
    std::swap(_drawCount, vao._drawCount);
    std::swap(_attachedVBOs, vao._attachedVBOs);
}

VertexArray::~VertexArray()
{
    destroy();
}

VertexArray& VertexArray::operator=(VertexArray&& vao)
{
    _isCreated = false;

    std::swap(_vaoID, vao._vaoID);
    std::swap(_isCreated, vao._isCreated);
    std::swap(_isDrawTargetSet, vao._isDrawTargetSet);
    std::swap(_isDrawCountSet, vao._isDrawCountSet);
    std::swap(_drawOffset, vao._drawOffset);
    std::swap(_drawTarget, vao._drawTarget);
    std::swap(_drawCount, vao._drawCount);
    std::swap(_attachedVBOs, vao._attachedVBOs);

    return *this;
}

void VertexArray::create()
{
    destroy();

    glGenVertexArrays(1, &_vaoID);
    _isCreated = true;

    setDrawOffset(0);
}

void VertexArray::destroy()
{
    if (isCreated()) {
        glDeleteVertexArrays(1, &_vaoID);

        _isCreated = false;
        _isDrawTargetSet = false;
        _isDrawCountSet = false;
    }
}

void VertexArray::bind() const
{
    glBindVertexArray(getID());
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::drawArrays() const
{
    if (_isDrawTargetSet && _isDrawCountSet) {
        glDrawArrays(static_cast<GLenum>(getDrawTarget()), getDrawOffset(), getDrawCount());
    } else {
        std::cerr
            << "base::gl::VertexArray > Attempt to use draw call (glDrawArrays) without setting target/offset/count";
    }
}

void VertexArray::enableAttrib(GLuint index)
{
    glEnableVertexAttribArray(index);
}

void VertexArray::disableAttrib(GLuint index)
{
    glDisableVertexAttribArray(index);
}

void VertexArray::attachVBO(const VertexBuffer* vertexBuffer)
{
    _attachedVBOs.push_back(vertexBuffer);
}

void VertexArray::setAttribPointer(
    GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void VertexArray::setAttribPointers()
{
    for (const VertexBuffer* vbo : _attachedVBOs) {
        if (vbo != nullptr)
            setAttribPointers(*vbo);
    }
}

void VertexArray::setAttribPointers(const VertexBuffer& vertexBuffer)
{
    vertexBuffer.bind();
    setAttribPointers(vertexBuffer.getAttributes());
    vertexBuffer.unbind();
}

void VertexArray::setAttribPointers(const std::vector<VertexAttrib>& attributes)
{
    for (const VertexAttrib& attrib : attributes) {
        enableAttrib(attrib.index);
        setAttribPointer(attrib.index, attrib.size, attrib.type, attrib.normalized, attrib.stride, attrib.offset);
    }
}

void VertexArray::setDrawOffset(GLint offset)
{
    _drawOffset = offset;
}

void VertexArray::setDrawCount(GLsizei count)
{
    _drawCount = count;
    _isDrawCountSet = true;
}

void VertexArray::setDrawTarget(DrawTarget target)
{
    _drawTarget = target;
    _isDrawTargetSet = true;
}

GLint VertexArray::getDrawOffset() const
{
    return _drawOffset;
}

GLsizei VertexArray::getDrawCount() const
{
    return _drawCount;
}

VertexArray::DrawTarget VertexArray::getDrawTarget() const
{
    return _drawTarget;
}

GLuint VertexArray::getID() const
{
    if (!isCreated())
        const_cast<VertexArray*>(this)->create();

    return _vaoID;
}

const std::list<const VertexBuffer*>& VertexArray::getAttachedVBOs() const
{
    return _attachedVBOs;
}

bool VertexArray::isCreated() const
{
    return _isCreated;
}
}
}
