
#pragma once

#include <list>

#include <glad/glad.h>

#include "base/utils.h"
#include "base/math.h"

/**
 * Technique represents the shader program 
*/
class GLTechnique
{
public:

    GLTechnique(const char* pVertexPath,const char* pFragmentPath);

    virtual ~GLTechnique();

    // Use the program
    void Enable() const;

    bool SetUniformFloat(const char* pUniformName,const float& value) const;
    bool SetUniformVec3(const char* pUniformName,const glm::vec3& value) const;
    bool SetUniformMat4(const char* pUniformName,const glm::mat4& value) const;
    bool SetSamplerUnit(const char* pUniformName,const unsigned int value) const;

    bool Valid() const { return m_shaderProgram != 0 && m_shaderProgram != INVALID_OGL_VALUE; }
    GLuint GetProgram() const { return m_shaderProgram; }
protected:

    bool AddShader(GLenum ShaderType, const char* pFilename);

    // Link the program
    bool Link();

    GLint GetUniformLocation(const char* pUniformName) const;

    GLuint m_shaderProgram = INVALID_OGL_VALUE;

private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
};

