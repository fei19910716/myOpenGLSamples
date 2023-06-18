#pragma once

// holds all OpenGL type declarations
#include <glad/glad.h> 

#include "base/gltechnique.h"
#include "base/utils.h"
#include "base/glvertexarray.h"

#include <vector>

using namespace std;

struct GLVertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

class GLMesh {
public:
    // constructor
    GLMesh(const vector<GLVertex>& vertices, const vector<unsigned int>& indices)
    {
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        GLVertexBuffer* VBO = GLVertexBuffer::builder().Attribute(AttributeType::POSITION, 0, 3, AttributeDataType::GLFLOAT, GL_FALSE, sizeof(GLVertex), 0)
                                                       .Attribute(AttributeType::Normal,   1, 3, AttributeDataType::GLFLOAT, GL_FALSE, sizeof(GLVertex), offsetof(GLVertex, Normal))
                                                       .Attribute(AttributeType::TexCoord, 2, 2, AttributeDataType::GLFLOAT, GL_FALSE, sizeof(GLVertex), offsetof(GLVertex, TexCoords))
                                                        .Attribute(AttributeType::Tangent,  3, 3, AttributeDataType::GLFLOAT, GL_FALSE, sizeof(GLVertex), offsetof(GLVertex, Tangent))
                                                        .Attribute(AttributeType::Bitangent,4, 3, AttributeDataType::GLFLOAT, GL_FALSE, sizeof(GLVertex), offsetof(GLVertex, Bitangent))
                                                        .Buffer(vertices.data())
                                                        .Size(vertices.size() * sizeof(GLVertex))
                                                        .VertexCount(vertices.size())
                                                        .build();

        GLIndexBuffer* EBO = GLIndexBuffer::builder().ElementType(IndexDataType::GLUNSIGNED_INT)
                                                     .IndexCount(static_cast<unsigned int>(indices.size()))
                                                     .Buffer(indices.data())
                                                     .IndexCount(indices.size())
                                                     .Size(indices.size() * sizeof(unsigned int))
                                                     .build();

        m_VAO = GLVertexArray::builder().VBO(VBO).EBO(EBO).build();
    }

    GLMesh(GLVertexArray* vao){
        m_VAO = vao;
    }

    ~GLMesh(){
        SAFE_DELETE(m_VAO);
    }

    // render the mesh
    void Draw(const GLTechnique* shader) 
    {
        if(m_VAO == nullptr){
            DEV_ERROR("m_VAO is nullptr!");
            return;
        }
        // draw mesh
        m_VAO->Bind();

        if(m_VAO->ArrayDraw() == false){
            // GLenum mode, GLsizei count, GLenum type
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_VAO->IndexCount()), m_VAO->IndexDataType(), 0);
        }else{
            // GLenum mode, GLint first, GLsizei count
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_VAO->VertexCount()));
        }

        glBindVertexArray(0);
    }

private:
    GLMesh() = delete;

    GLVertexArray* m_VAO = nullptr;
};
