
#pragma once

#include <list>

#include <glad/glad.h>

#include "base/utils.h"

/**
 * Technique represents the shader program 
*/
class Technique
{
public:

    Technique(const char* pVertexPath,const char* pFragmentPath);

    virtual ~Technique();

    // Use the program
    void Enable();

    bool Valid() const { return m_shaderProgram != 0 && m_shaderProgram != INVALID_OGL_VALUE; }

protected:

    bool AddShader(GLenum ShaderType, const char* pFilename);

    // Link the program
    bool Link();

    GLint GetUniformLocation(const char* pUniformName);

    GLuint m_shaderProgram = INVALID_OGL_VALUE;

private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
};

