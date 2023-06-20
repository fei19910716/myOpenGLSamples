
#pragma once

#include <list>

#include <glad/glad.h>

#include "base/utils.h"
#include "base/math.h"
#include "base/globject.h"

/**
 * Technique represents the shader program 
*/
class GLTechnique: public GLObject
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

    bool Valid() const { return m_ID != 0 && m_ID != INVALID_OGL_VALUE; }

protected:

    bool AddShader(GLenum ShaderType, const char* pFilename);

    // Link the program
    bool Link();

    GLint GetUniformLocation(const char* pUniformName) const;


private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
};

