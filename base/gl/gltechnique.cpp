
#include "gltechnique.h"

#include <string.h>
#include "base/exception.h"

GLTechnique::GLTechnique(const char* pVertexPath, const char* pFragmentPath)
{
    if(pVertexPath == nullptr){
        DEV_ERROR("VertexPath is null");
        return;
    }

    if(pFragmentPath == nullptr){
        DEV_ERROR("VertexPath is null");
        return;
    }

    m_ID = glCreateProgram();

    if (m_ID == 0) {
        DEV_ERROR("Error creating shader program\n");
        return;
    }

    if(!AddShader(GL_VERTEX_SHADER,pVertexPath)){
        return;
    }

    if(!AddShader(GL_FRAGMENT_SHADER,pFragmentPath)){
        return;
    }
    

    if(!Link()){
        return;
    }
}


GLTechnique::~GLTechnique()
{
    // Delete the intermediate shader objects that have been added to the program
    // The list will only contain something if shaders were compiled but the object itself
    // was destroyed prior to linking.
    for (ShaderObjList::iterator it = m_shaderObjList.begin() ; it != m_shaderObjList.end() ; it++)
    {
        glDeleteShader(*it);
    }

    if (m_ID != 0)
    {
        glDeleteProgram(m_ID);
        m_ID = 0;
    }
}

// Use this method to add shaders to the program. When finished - call finalize()
bool GLTechnique::AddShader(GLenum ShaderType, const char* pFilename)
{
    std::string s;

    if (!UTILS::ReadFile(pFilename, s)) {
        return false;
    }

    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        DEV_ERROR("Error creating shader type %d\n", ShaderType);
        return false;
    }

    // Save the shader object - will be deleted in the destructor
    m_shaderObjList.push_back(ShaderObj);

    const GLchar* p[1];
    p[0] = s.c_str();
    GLint Lengths[1] = { (GLint)s.size() };

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        DEV_ERROR("Error compiling '%s': '%s'\n", pFilename, InfoLog);
        return false;
    }

    glAttachShader(m_ID, ShaderObj);

    return GLCheckError();
}


// After all the shaders have been added to the program call this function
// to link and validate the program.
bool GLTechnique::Link()
{
    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(m_ID);

    glGetProgramiv(m_ID, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(m_ID, sizeof(ErrorLog), NULL, ErrorLog);
        DEV_ERROR("Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(m_ID);
    glGetProgramiv(m_ID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(m_ID, sizeof(ErrorLog), NULL, ErrorLog);
        DEV_ERROR("Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }

    // Delete the intermediate shader objects that have been added to the program
    for (ShaderObjList::iterator it = m_shaderObjList.begin() ; it != m_shaderObjList.end() ; it++) {
        glDeleteShader(*it);
    }

    m_shaderObjList.clear();

    return GLCheckError();
}


void GLTechnique::Enable() const
{
    if(!Valid()){
        DEV_ERROR("program is invalid.");
        return;
    }
    glUseProgram(m_ID);
}


GLint GLTechnique::GetUniformLocation(const char* pUniformName) const
{
    if(!Valid()){
        DEV_ERROR("program is invalid.");
        return INVALID_UNIFORM_LOCATION;
    }

    return glGetUniformLocation(m_ID, pUniformName);
}

bool GLTechnique::SetUniformFloat(const char* pUniformName,const float& value) const{
    GLint location = GetUniformLocation(pUniformName);
    glUniform1f(location,value);

    return true;
}

bool GLTechnique::SetUniformVec3(const char* pUniformName,const glm::vec3& value) const{
    GLint location = GetUniformLocation(pUniformName);
    glUniform3fv(location, 1, glm::value_ptr(value));

    return true;
}

bool GLTechnique::SetUniformMat4(const char* pUniformName,const glm::mat4& value) const{
    GLint location = GetUniformLocation(pUniformName);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));

    return true;
}

 bool GLTechnique::SetSamplerUnit(const char* pUniformName,const unsigned int value) const{
    GLint location = GetUniformLocation(pUniformName);

    if (location == INVALID_UNIFORM_LOCATION) {
        return false;
    }

    glUniform1i(location,value);

    return true;
 }
