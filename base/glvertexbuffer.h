#pragma once
#include "base/types.h"
#include "base/utils.h"

#include <glad/glad.h>

#define MAX_VERTEX_ATTRIBUTE_COUNT 16

enum AttributeType : uchar {
    POSITION,
    Normal,
    COLOR,
    TexCoord,
    Tangent,
    Bitangent
};

enum class AttributeDataType {
    GLBYTE = 0x1400,
    GLUNSIGNED_BYTE = 0x1401,
    GLSHORT = 0x1402,
    GLUNSIGNED_SHORT = 0x1403,
    GLINT = 0x1404,
    GLUNSIGNED_INT = 0x1405,
    GLFLOAT = 0x1406
};

class GLVertexArray;

struct VertexAttribute {
    bool BUFFER_USED = false;
    bool normalized  = false;
    uint8_t  count   = 0;
    uint8_t  offset  = 0;
    uint8_t  stride  = 0;
    uint8_t  bufferIndex = 0;
    AttributeDataType dataType = AttributeDataType::GLBYTE;
};

class GLVertexBuffer{
public:

    static GLVertexBuffer& builder() noexcept{
        GLVertexBuffer* buffer = new GLVertexBuffer;
        return *buffer;
    }

    ~GLVertexBuffer(){
        glDeleteBuffers(1,&m_ID);
    }

    GLVertexBuffer& Attribute(AttributeType attribute, uint8_t bufferIndex, uint8_t count,
                AttributeDataType dataType, bool normalized, uint8_t byteStride = 0, uint8_t byteOffset = 0) noexcept
    {
        VertexAttribute& entry = mAttributes[attribute];
        entry.bufferIndex = bufferIndex;
        entry.count = count;
        entry.dataType = dataType;
        entry.normalized = normalized;
        entry.stride = byteStride;
        entry.offset = byteOffset;
        
        entry.BUFFER_USED = true;

        return *this;
    }

    GLVertexBuffer& VertexCount(size_t count = 0) noexcept
    {
        m_vertexCount = count;

        return *this;
    }

    GLVertexBuffer& Buffer(void const* buffer) noexcept
    {
        m_buffer = buffer;

        return *this;
    }

    GLVertexBuffer& Size(size_t size) noexcept
    {
        m_size = size;

        return *this;
    }

    GLVertexBuffer* build()
    {
        glGenBuffers(1, &m_ID);
        return this;
    }

    unsigned int GetID() const{
        return m_ID;
    }

private:
    void const*     m_buffer;
    size_t          m_size;
    size_t          m_vertexCount;
    unsigned int    m_ID;

    VertexAttribute mAttributes[MAX_VERTEX_ATTRIBUTE_COUNT];
    friend GLVertexArray;
};