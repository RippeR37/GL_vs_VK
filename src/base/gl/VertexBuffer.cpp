#include <base/gl/VertexBuffer.h>

namespace base {
namespace gl {
VertexBuffer::VertexBuffer()
    : Buffer(Target::Array)
{
}

VertexBuffer::VertexBuffer(Usage usage)
    : Buffer(Target::Array, usage)
{
}

VertexBuffer::VertexBuffer(VertexBuffer&& vbo)
    : Buffer(std::move(vbo))
{
    std::swap(_attributePointers, vbo._attributePointers);
}

VertexBuffer::~VertexBuffer()
{
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vbo)
{
    std::swap(_usage, vbo._usage);
    std::swap(_target, vbo._target);
    std::swap(_bufferID, vbo._bufferID);
    std::swap(_isCreated, vbo._isCreated);
    std::swap(_attributePointers, vbo._attributePointers);

    return *this;
}

void VertexBuffer::bind() const
{
    glBindBuffer(static_cast<GLenum>(Buffer::Target::Array), getID());
}

void VertexBuffer::setData(const VertexBuffer::Data& data)
{
    Buffer::setData(data.size, data.data);
    setAttributes(data.pointers);
}

void VertexBuffer::setTarget()
{
    _target = Target::Array;
}

void VertexBuffer::setAttributes(std::vector<VertexAttrib> attributes)
{
    _attributePointers = std::move(attributes);
}

Buffer::Target VertexBuffer::getTarget() const
{
    return Target::Array;
}

const std::vector<VertexAttrib>& VertexBuffer::getAttributes() const
{
    return _attributePointers;
}
}
}
