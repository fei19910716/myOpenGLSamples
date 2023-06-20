

#include <iostream>

#include "utils.h"
#include "gltexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

GLTexture::GLTexture(GLenum TextureTarget, const std::vector<std::string>& files, TextureType type):
m_textureTarget(TextureTarget),
m_textureType(type)
{
    assert(m_textureTarget == GL_TEXTURE_CUBE_MAP);
    assert(files.size() == 6);

    LoadCubemap(files);
}

GLTexture::GLTexture(GLenum TextureTarget, const std::string& file, TextureType type):
m_textureTarget(TextureTarget),
m_texture2DPath(file),
m_textureType(type)
{
    assert(m_textureTarget == GL_TEXTURE_2D);

    if(!file.empty()){
        LoadTexture2D(file);
    }
    
}

GLTexture::~GLTexture(){
   glDeleteTextures(1,&m_ID);
}


bool GLTexture::LoadTexture2D(u32 BufferSize, void* pData)
{
    void* image_data = stbi_load_from_memory((const stbi_uc*)pData, BufferSize, &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);

    if(!LoadInternal(image_data)){
        return false;
    }

    stbi_image_free(image_data);

    return true;
}


bool GLTexture::LoadTexture2D(const std::string& Filename)
{
    assert(m_textureTarget == GL_TEXTURE_2D);

    stbi_set_flip_vertically_on_load(1);

    unsigned char* image_data = stbi_load(Filename.c_str(), &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);

    if (!image_data) {
        DEV_ERROR("Can't load texture from '%s' - %s\n", Filename.c_str(), stbi_failure_reason());
        return false;
    }

    printf("Name: %s, Width %d, height %d, bpp %d\n", Filename.c_str(), m_imageWidth, m_imageHeight, m_imageBPP);

    if(!LoadInternal(image_data)){
        return false;
    }

    stbi_image_free(image_data);

    return true;
}


bool GLTexture::LoadTexture2D(int Width, int Height, int BPP, unsigned char* pData)
{
    m_imageWidth = Width;
    m_imageHeight = Height;
    m_imageBPP = BPP;

    return LoadInternal(pData);
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
bool GLTexture::LoadCubemap(const std::vector<std::string>& faces)
{
    glGenTextures(1, &m_ID);
    glBindTexture(m_textureTarget, m_ID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            DEV_ERROR("Cubemap texture failed to load at path: %s",faces[i].c_str());
            stbi_image_free(data);
            return false;
        }
    }
    glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}


bool GLTexture::LoadInternal(void* image_data)
{
    assert(m_textureTarget == GL_TEXTURE_2D);

    glGenTextures(1, &m_ID);
    glBindTexture(m_textureTarget, m_ID);

    switch (m_imageBPP) {
        case 1:
            glTexImage2D(m_textureTarget, 0, GL_RED, m_imageWidth, m_imageHeight, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
            break;

        case 3:
            glTexImage2D(m_textureTarget, 0, GL_RGB, m_imageWidth, m_imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
            break;

        case 4:
            glTexImage2D(m_textureTarget, 0, GL_RGBA, m_imageWidth, m_imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            break;

        default:
            DEV_ERROR("Not implemented case\n");
            return false;
    }

    glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(m_textureTarget);

    glBindTexture(m_textureTarget, 0);

    return GLCheckError();
}

bool GLTexture::Bind(GLenum TextureUnit) const
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_ID);

    return GLCheckError();
}
