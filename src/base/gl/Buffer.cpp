#include <base/gl/Buffer.h>

namespace base {
namespace gl {
Buffer::Buffer()
    : Buffer(Target::Array, Usage::StaticDraw)
{
}

Buffer::Buffer(Target target, Usage usage)
{
    _isCreated = false;
    _bufferID = 0;

    setTarget(target);
    setUsage(usage);
}

Buffer::Buffer(Buffer&& buffer)
{
    _isCreated = false;

    std::swap(_usage, buffer._usage);
    std::swap(_target, buffer._target);
    std::swap(_bufferID, buffer._bufferID);
    std::swap(_isCreated, buffer._isCreated);
}

Buffer::~Buffer()
{
    destroy();
}

Buffer& Buffer::operator=(Buffer&& buffer)
{
    std::swap(_usage, buffer._usage);
    std::swap(_target, buffer._target);
    std::swap(_bufferID, buffer._bufferID);
    std::swap(_isCreated, buffer._isCreated);

    return *this;
}

void Buffer::create()
{
    destroy();

    glGenBuffers(1, &_bufferID);
    _isCreated = true;
}

void Buffer::destroy()
{
    if (isCreated()) {
        glDeleteBuffers(1, &_bufferID);

        _isCreated = false;
    }
}

void Buffer::bind() const
{
    glBindBuffer(static_cast<GLenum>(getTarget()), getID());
}

void Buffer::bind(Target target)
{
    setTarget(target);
    bind();
}

void Buffer::unbind() const
{
    glBindBuffer(static_cast<GLenum>(getTarget()), 0);
}

void Buffer::resize(GLsizeiptr size)
{
    glBufferData(static_cast<GLenum>(getTarget()), size, nullptr, static_cast<GLenum>(getUsage()));
}

void Buffer::setData(const Buffer::Data& data)
{
    setData(data.size, data.data);
}

void Buffer::setData(GLsizeiptr size, const GLvoid* data)
{
    glBufferData(static_cast<GLenum>(getTarget()), size, data, static_cast<GLenum>(getUsage()));
}

void Buffer::setData(GLsizeiptr size, const GLvoid* data, Usage usage)
{
    setUsage(usage);
    glBufferData(static_cast<GLenum>(getTarget()), size, data, static_cast<GLenum>(getUsage()));
}

void Buffer::setSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    glBufferSubData(static_cast<GLenum>(getTarget()), offset, size, data);
}

void Buffer::setUsage(Usage usage)
{
    _usage = usage;
}

void Buffer::setTarget(Target target)
{
    _target = target;
}

GLuint Buffer::getID() const
{
    if (!isCreated())
        const_cast<Buffer*>(this)->create();

    return _bufferID;
}

Buffer::Usage Buffer::getUsage() const
{
    return _usage;
}

Buffer::Target Buffer::getTarget() const
{
    return _target;
}

bool Buffer::isCreated() const
{
    return _isCreated;
}
}
}
