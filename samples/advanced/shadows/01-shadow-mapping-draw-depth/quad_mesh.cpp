
#include "quad_mesh.h"

#include "base/utils.h"
#include "base/math_3d.h"

bool QuadMesh::Init()
{
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

             -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));    
    
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
    if (VAO != INVALID_OGL_VALUE)
    {
        glDeleteVertexArrays(1, &VAO);
    }

    if(VBO != INVALID_OGL_VALUE){
        glDeleteBuffers(1,&VBO);
    }
}


void QuadMesh::Render()
{
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
