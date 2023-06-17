#pragma once
#include "base/types.h"
#include "base/utils.h"

enum IndexElementType : uint8_t {
    USHORT,
    UINT
};

class GLIndexBuffer{
public:

    GLIndexBuffer() = default;

    static GLIndexBuffer& builder() noexcept{
        GLIndexBuffer* buffer = new GLIndexBuffer;
        return *buffer;
    }

    GLIndexBuffer& IndexCount(uint8_t count = 0) noexcept
    {
        indexCount = count;

        return *this;
    }

    GLIndexBuffer& ElementType(IndexElementType type = IndexElementType::USHORT) noexcept
    {
        elementType = type;

        return *this;
    }

    GLIndexBuffer* build(void const* buffer, size_t size)
    {
        glGenBuffers(1, &EBO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);

        return this;
    }

    unsigned int GetID() const{
        return EBO;
    }

private:

    unsigned int EBO;

    uint8_t indexCount = 0;
    IndexElementType elementType = IndexElementType::USHORT;
};