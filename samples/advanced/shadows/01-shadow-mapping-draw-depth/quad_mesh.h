
#pragma once

#include "base/dev_gl.h"

class QuadMesh
{
public:
    QuadMesh();

    ~QuadMesh();

    bool Init();

    void Render();

private:
    void Clear();

    GLuint m_VB;
};