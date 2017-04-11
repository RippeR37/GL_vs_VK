#pragma once

#include <GL/glew.h>

#include <functional>
#include <list>
#include <vector>

namespace base {
namespace gl {
class Buffer
{
  public:
    enum class Usage
    {
        StaticDraw = GL_STATIC_DRAW,
        StaticRead = GL_STATIC_READ,
        StaticCopy = GL_STATIC_COPY,
        DynamicDraw = GL_DYNAMIC_DRAW,
        DynamicRead = GL_DYNAMIC_READ,
        DynamicCopy = GL_DYNAMIC_COPY,
        StreamDraw = GL_STREAM_DRAW,
        StreamRead = GL_STREAM_READ,
        StreamCopy = GL_STREAM_COPY,
    };

    enum class Target
    {
        Array = GL_ARRAY_BUFFER,
        AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
        CopyRead = GL_COPY_READ_BUFFER,
        CopyWrite = GL_COPY_WRITE_BUFFER,
        DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
        DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
        ElementArray = GL_ELEMENT_ARRAY_BUFFER,
        PixelPack = GL_PIXEL_PACK_BUFFER,
        PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
        Query = GL_QUERY_BUFFER,
        ShaderStorage = GL_SHADER_STORAGE_BUFFER,
        Texture = GL_TEXTURE_BUFFER,
        TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
        Uniform = GL_UNIFORM_BUFFER,
    };

  public:
    class Data
    {
      public:
        Data()
            : data(nullptr)
            , size(0)
        {
        }

        Data(GLvoid* data_, GLsizeiptr size_)
            : data(data_)
            , size(size_)
        {
        }

        GLvoid* data;
        GLsizeiptr size;
    };

  public:
    Buffer();
    Buffer(Target target, Usage usage = Usage::StaticDraw);
    Buffer(Buffer&& buffer);
    Buffer(const Buffer&) = delete;
    virtual ~Buffer();

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&& buffer);

    void create();
    void destroy();

    virtual void bind() const;
    void bind(Target target);
    void unbind() const;

    void resize(GLsizeiptr size);

    virtual void setData(const Buffer::Data& data);
    void setData(GLsizeiptr size, const GLvoid* data);
    void setData(GLsizeiptr size, const GLvoid* data, Usage usage);
    void setSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data);

    void setUsage(Usage usage);
    virtual void setTarget(Target target);

    GLuint getID() const;
    Usage getUsage() const;
    virtual Target getTarget() const;

  public:
    template <typename T>
    void setData(const std::vector<T>& data);

    template <typename T>
    void setData(const std::vector<T>& data, Usage usage);

    template <typename T>
    void setSubData(const std::vector<T>& data, GLintptr offset);

    template <typename T>
    void setSubData(const std::vector<T>& data, GLintptr offset, GLsizeiptr size);

  protected:
    bool isCreated() const;

    bool _isCreated;
    GLuint _bufferID;
    Target _target;
    Usage _usage;
};

template <typename T>
void Buffer::setData(const std::vector<T>& data)
{
    glBufferData(static_cast<GLenum>(getTarget()), data.size() * sizeof(T), data.data(),
                 static_cast<GLenum>(getUsage()));
}

template <typename T>
void Buffer::setData(const std::vector<T>& data, Usage usage)
{
    setUsage(usage);
    setData(data);
}

template <typename T>
void Buffer::setSubData(const std::vector<T>& data, GLintptr offset)
{
    setSubData(data, offset, data.size() * sizeof(T));
}

template <typename T>
void Buffer::setSubData(const std::vector<T>& data, GLintptr offset, GLsizeiptr size)
{
    glBufferSubData(static_cast<GLenum>(getTarget()), offset, size, &data.data());
}
}
}
