
#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include <list>

#include <glad/glad.h>

/**
 * Technique represents the shader program 
*/
class Technique
{
public:

    Technique();

    virtual ~Technique();

    virtual bool Init();

    // Use the program
    void Enable();

    GLuint GetProgram() const { return m_shaderProg; }

protected:

    bool AddShader(GLenum ShaderType, const char* pFilename);

    // Link the program
    bool Finalize();

    GLint GetUniformLocation(const char* pUniformName);

    GLuint m_shaderProg = 0;

private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
};

#endif  /* TECHNIQUE_H */
