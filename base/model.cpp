#include "model.h"

namespace MODEL{
// constructor, expects a filepath to a 3D model.
Model::Model(string const &path, bool gamma) : 
gammaCorrection(gamma)
{
    LoadModel(path);
}

Model::~Model(){
    for(auto item: meshes){
        delete item;
    }

    for(auto item: textures_loaded){
        delete item;
    }
}

// draws the model, and thus all its meshes
void Model::Draw(const GLTechnique* shader)
{   
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(unsigned int i = 0; i < textures_loaded.size(); i++)
    {
        // retrieve texture number (the N in diffuse_textureN)
        std::string samplerName;
        TextureType type = textures_loaded[i]->GetTextureType();
        switch (type)
        {
        case TextureType::DiffuseMap:
            samplerName = "texture_diffuse" + std::to_string(diffuseNr++);
            break;
        case TextureType::SpeculerMap:
            samplerName = "texture_specular" + std::to_string(diffuseNr++);
            break;
        case TextureType::HeightMap:
            samplerName = "texture_height" + std::to_string(diffuseNr++);
            break;
        case TextureType::NormalMap:
            samplerName = "texture_normal" + std::to_string(diffuseNr++);
            break;
        
        default:
            break;
        }

        if(textures_loaded[i]->Bind(GL_TEXTURE0 + i) == false){
            DEV_ERROR("Bind texture fail!");
        }
        
        if(shader->SetSamplerUnit(samplerName.c_str(), i) == false){
            //DEV_ERROR("Uniform location invalid '%s'\n", samplerName.c_str());
        }

    }
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i]->Draw(shader);
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::LoadModel(string const &path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/')).append("/");

    // process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }

}

GLMesh* Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    vector<GLVertex> vertices;
    vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        GLVertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    LoadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DiffuseMap);
    // 2. specular maps
    LoadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SpeculerMap);
    // 3. normal maps
    LoadMaterialTextures(material, aiTextureType_NORMALS, TextureType::NormalMap);
    // 4. height maps
    LoadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::HeightMap);
    
    // return a mesh object created from the extracted mesh data
    return new GLMesh(vertices, indices);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
void Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType aiType, TextureType devType)
{
    for(unsigned int i = 0; i < mat->GetTextureCount(aiType); i++)
    {
        aiString str;
        mat->GetTexture(aiType, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        std::string path = directory + std::string(str.C_Str());
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(textures_loaded[j]->GetFilePath().compare(path) ==0)
            {
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   
            // if texture hasn't been loaded already, load it
            GLTexture* texture = new GLTexture(GL_TEXTURE_2D, path.c_str(), devType);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
}

}