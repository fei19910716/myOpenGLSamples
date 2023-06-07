
#pragma once

#include "base/dev_gl.h"

class GroundMesh
{
public:
    GroundMesh();

    ~GroundMesh();

    bool Init();

    void Render();

private:
    void Clear();

    GLuint m_VB;
};