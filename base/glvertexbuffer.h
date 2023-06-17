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

enum class AttributeElementType : uchar {
    BYTE,
    BYTE2,
    BYTE3,
    BYTE4,
    UBYTE,
    UBYTE2,
    UBYTE3,
    UBYTE4,
    SHORT,
    SHORT2,
    SHORT3,
    SHORT4,
    USHORT,
    USHORT2,
    USHORT3,
    USHORT4,
    INT,
    UINT,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
    HALF,
    HALF2,
    HALF3,
    HALF4,
};



struct Attribute {
    bool BUFFER_USED = false;
    bool normalized  = false;
    uint8_t  count   =0;
    uint32_t offset = 0;
    uint8_t stride = 0;
    uint8_t buffer = -1;
    AttributeElementType type = AttributeElementType::BYTE;
};

class GLVertexBuffer{
public:
    Attribute mAttributes[MAX_VERTEX_ATTRIBUTE_COUNT];

    static GLVertexBuffer& builder() noexcept{
        GLVertexBuffer* buffer = new GLVertexBuffer;
        return *buffer;
    }

    GLVertexBuffer& Attribute(AttributeType attribute, uint8_t bufferIndex,
                AttributeElementType attributeType,
                uint32_t byteOffset = 0, uint8_t byteStride = 0) noexcept
    {
        Attribute& entry = mAttributes[attribute];
        entry.buffer = bufferIndex;
        entry.offset = byteOffset;
        entry.stride = byteStride;
        entry.type = attributeType;
        entry.BUFFER_USED = true;
    }

    GLVertexBuffer* build(void const* buffer, size_t size)
    {
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
        
        for (size_t j = 0; j < MAX_VERTEX_ATTRIBUTE_COUNT; ++j) {
            Attribute& entry = mAttributes[j];
            if (entry.BUFFER_USED) {
                glEnableVertexAttribArray(j);
                glVertexAttribPointer(j, entry.count, entry.type, entry.normalized, entry.stride, entry.offset);
            }
        }
    }

    unsigned int GetID() const{
        return VBO;
    }

private:

    unsigned int VBO;
}