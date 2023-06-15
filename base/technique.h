
#pragma once

#include <list>

#include <glad/glad.h>

#include "base/utils.h"
#include "base/math.h"

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

    bool SetUniformFloat(const char* pUniformName,const float& value);
    bool SetUniformVec3(const char* pUniformName,const glm::vec3& value);
    bool SetUniformMat4(const char* pUniformName,const glm::mat4& value);
    bool SetSamplerUnit(const char* pUniformName,const unsigned int value);

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

