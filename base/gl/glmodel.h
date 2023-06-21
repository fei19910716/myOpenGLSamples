#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "base/gl/glmesh.h"
#include "base/gl/gltexture.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;


class GLModel 
{
public:
    // model data 
    vector<GLTexture*> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<GLMesh*>    meshes;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    GLModel(string const &path, bool gamma = false);

    ~GLModel();

    // draws the model, and thus all its meshes
    void Draw(const GLTechnique* shader);
    
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void LoadModel(string const &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void ProcessNode(aiNode *node, const aiScene *scene);

    GLMesh* ProcessMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    void LoadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType devType);
};

