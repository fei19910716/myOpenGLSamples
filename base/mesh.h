#pragma once

// holds all OpenGL type declarations
#include <glad/glad.h> 

#include "base/technique.h"
#include "base/texture.h"

#include <string>
#include <vector>

using namespace std;

namespace MODEL{

struct Vertex {
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

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture*>      textures;
    unsigned int         VAO, VBO, EBO;

    // constructor
    Mesh(const vector<Vertex>& vertices, const vector<unsigned int>& indices, const vector<Texture*>& textures)
    {
        this->vertices = vertices;
        this->indices  = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        SetupMesh();
    }

    ~Mesh(){
        glDeleteVertexArrays(1,&VAO);
        glDeleteBuffers(1,&VBO);
        glDeleteBuffers(1,&EBO);
    }

    // render the mesh
    void Draw(const Technique* shader) 
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            // retrieve texture number (the N in diffuse_textureN)
            std::string samplerName;
            TextureType type = textures[i]->GetTextureType();
            switch (type)
            {
            case TextureType::Diffuse:
                samplerName = "texture_diffuse" + std::to_string(diffuseNr++);
                break;
            case TextureType::Speculer:
                samplerName = "texture_specular" + std::to_string(diffuseNr++);
                break;
            case TextureType::Height:
                samplerName = "texture_height" + std::to_string(diffuseNr++);
                break;
            case TextureType::Normal:
                samplerName = "texture_normal" + std::to_string(diffuseNr++);
                break;
            
            default:
                break;
            }
            textures[i]->Bind(GL_TEXTURE0 + i);
            shader->SetSamplerUnit(samplerName.c_str(), i);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:

    // initializes all the buffer objects/arrays
    void SetupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    }
};

}