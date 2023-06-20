#pragma once

#include <glad/glad.h>

class GLObject{
public:
    GLObject() = default;
    virtual ~GLObject() = default;

    GLuint GetID() const {
        return m_ID;
    }

public:
    unsigned int m_ID;
};