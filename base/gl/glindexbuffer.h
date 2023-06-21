#pragma once

#include "base/types.h"
#include "base/utils.h"

#include "base/gl/globject.h"

#define MAX_VERTEX_ATTRIBUTE_COUNT 16

enum IndexDataType{
    GLSHORT = 0x1402,
    GLUNSIGNED_SHORT = 0x1403,
    GLINT = 0x1404,
    GLUNSIGNED_INT = 0x1405
};

class GLVertexArray;
class GLIndexBuffer: public GLObject{
public:
    static GLIndexBuffer& builder() noexcept{
        GLIndexBuffer* buffer = new GLIndexBuffer;
        return *buffer;
    }

    ~GLIndexBuffer(){
        glDeleteBuffers(1,&m_ID);
    }

    GLIndexBuffer& IndexCount(size_t count = 0) noexcept
    {
        m_indexCount = count;

        return *this;
    }

    GLIndexBuffer& ElementType(IndexDataType type = IndexDataType::GLUNSIGNED_SHORT) noexcept
    {
        m_elementType = type;

        return *this;
    }

    GLIndexBuffer& Buffer(void const* buffer) noexcept
    {
        m_buffer = buffer;

        return *this;
    }

    GLIndexBuffer& Size(size_t size) noexcept
    {
        m_size = size;

        return *this;
    }


    GLIndexBuffer* build()
    {
        glGenBuffers(1, &m_ID);

        return this;
    }

private:
    GLIndexBuffer() = default;

    void const*     m_buffer;
    size_t          m_size;
    size_t          m_indexCount = 0;
    IndexDataType   m_elementType = IndexDataType::GLUNSIGNED_SHORT;

    friend GLVertexArray;
};