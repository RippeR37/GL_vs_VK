#pragma once

#include <base/gl/Buffer.h>
#include <base/gl/VertexAttrib.h>

#include <vector>

namespace base {
namespace gl {
    class VertexBuffer : public Buffer {
    public:
        class Data {
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
            std::vector<VertexAttrib> pointers;
        };

    public:
        VertexBuffer();
        VertexBuffer(Usage usage);
        VertexBuffer(VertexBuffer&& vbo);
        VertexBuffer(const VertexBuffer&) = delete;
        ~VertexBuffer();

        VertexBuffer& operator=(const VertexBuffer&) = delete;
        VertexBuffer& operator=(VertexBuffer&& vbo);

        void bind() const;

        void setData(const VertexBuffer::Data& data);

        void setTarget();
        void setAttributes(std::vector<VertexAttrib> attributes);

        Target getTarget() const;
        const std::vector<VertexAttrib>& getAttributes() const;

        using Buffer::setData;

    private:
        std::vector<VertexAttrib> _attributePointers;

        using Buffer::setTarget;
    };
}
}
