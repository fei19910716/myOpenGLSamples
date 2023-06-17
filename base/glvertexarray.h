#pragma once

#include "base/glvertexbuffer.h"
#include "base/glindexbuffer.h"

class GLVertexArray{
public:
    GLVertexArray() = delete;
    ~GLVertexArray(){
        glDeleteVertexArrays(1, &VAO);
    }

    static GLVertexArray& builder() noexcept{
        GLVertexArray* buffer = new GLVertexArray;
        return *buffer;
    }

    GLVertexArray& VBO(GLVertexBuffer* VBO) noexcept
    {
        if(this->VBO != VBO){
            delete this->VBO;
        }
        this->VBO = VBO;

        return *this;
    }

    GLVertexArray& EBO(GLIndexBuffer* EBO) noexcept
    {
        if(this->EBO != EBO){
            delete this->builder;
        }
        this->EBO = EBO;

        return *this;
    }

    GLVertexArray* build(){
        glGenVertexArrays(1, &VAO);

        glBindVertexArray(VAO);
        VBO && glBindBuffer(GL_ARRAY_BUFFER, VBO->GetID());
        EBO && glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO->GetID());

        glBindVertexArray(0);
    }

    void Bind() const {
        glBindVertexArray(VAO);
    }

private:
    unsigned int VAO;

    GLVertexBuffer* VBO;
    GLIndexBuffer*  EBO;
};