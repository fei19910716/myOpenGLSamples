
#include "quad_mesh.h"

#include "base/utils.h"
#include "base/math_3d.h"

bool QuadMesh::Init()
{
    float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top left
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
             
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    };

    glGenBuffers(1, &m_VB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    return true;
}

QuadMesh::QuadMesh()
{
}


QuadMesh::~QuadMesh()
{
    Clear();
}


void QuadMesh::Clear()
{
    if (m_VB != INVALID_OGL_VALUE)
    {
        glDeleteBuffers(1, &m_VB);
    }
}


void QuadMesh::Render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));         
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
