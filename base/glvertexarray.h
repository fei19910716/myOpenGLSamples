#pragma once

#include "base/glvertexbuffer.h"
#include "base/glindexbuffer.h"

class GLVertexArray{
public:
    GLVertexArray() = default;
    ~GLVertexArray(){
        glDeleteVertexArrays(1, &m_ID);
        SAFE_DELETE(m_VBO);
        SAFE_DELETE(m_EBO);
    }

    static GLVertexArray& builder() noexcept{
        GLVertexArray* buffer = new GLVertexArray;
        return *buffer;
    }

    GLVertexArray& VBO(GLVertexBuffer* vbo) noexcept
    {
        if(m_VBO != vbo){
            SAFE_DELETE(m_VBO);
        }
        m_VBO = vbo;

        return *this;
    }

    GLVertexArray& EBO(GLIndexBuffer* ebo) noexcept
    {
        if(m_EBO != ebo){
            SAFE_DELETE(m_EBO);
        }
        m_EBO = ebo;

        return *this;
    }

    GLVertexArray* build(){
        glGenVertexArrays(1, &m_ID);
        glBindVertexArray(m_ID);
        if(m_VBO) {
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO->GetID());
            glBufferData(GL_ARRAY_BUFFER, m_VBO->m_size, m_VBO->m_buffer, GL_STATIC_DRAW);
        
            for (GLuint j = 0; j < MAX_VERTEX_ATTRIBUTE_COUNT; ++j) {
                VertexAttribute& entry = m_VBO->mAttributes[j];
                if (entry.BUFFER_USED) {
                    glEnableVertexAttribArray(entry.bufferIndex);
                    glVertexAttribPointer(entry.bufferIndex, entry.count, (GLenum)entry.dataType, entry.normalized, entry.stride, (void*)(entry.offset));
                }
            }
        }
        if(m_EBO) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO->GetID());
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_EBO->m_size, m_EBO->m_buffer, GL_STATIC_DRAW);
        }


        glBindVertexArray(0);
        return this;
    }

    void Bind() const {
        glBindVertexArray(m_ID);
    }

    bool ArrayDraw() const{
        return m_EBO == nullptr;
    }

    size_t IndexCount() const{
        return m_EBO->m_indexCount;
    }

    IndexDataType IndexDataType() const{
        return m_EBO->m_elementType;
    }
    size_t VertexCount() const{
        return m_VBO->m_vertexCount;
    }

public:
    unsigned int    m_ID;
    GLVertexBuffer* m_VBO = nullptr;
    GLIndexBuffer*  m_EBO = nullptr;
};