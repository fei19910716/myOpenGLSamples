

#include <iostream>

#include "utils.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

Texture::Texture(GLenum TextureTarget, const std::string& FileName, TextureType type)
{
    m_textureTarget = TextureTarget;
    m_filePath      = FileName;
    m_textureType   = type;

    if(FileName.empty()){
        DEV_ERROR("FileName is empty");
        return;
    }
    
    Load(m_filePath);
}

Texture::~Texture(){
   glDeleteTextures(1,&m_ID);
}


bool Texture::Load(u32 BufferSize, void* pData)
{
    void* image_data = stbi_load_from_memory((const stbi_uc*)pData, BufferSize, &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);

    if(!LoadInternal(image_data)){
        return false;
    }

    stbi_image_free(image_data);

    return true;
}


bool Texture::Load(const std::string& Filename)
{
    stbi_set_flip_vertically_on_load(1);

    unsigned char* image_data = stbi_load(m_filePath.c_str(), &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);

    if (!image_data) {
        DEV_ERROR("Can't load texture from '%s' - %s\n", m_filePath.c_str(), stbi_failure_reason());
        return false;
    }

    printf("Name: %s, Width %d, height %d, bpp %d\n", Filename.c_str(), m_imageWidth, m_imageHeight, m_imageBPP);

    if(!LoadInternal(image_data)){
        return false;
    }

    stbi_image_free(image_data);

    return true;
}


bool Texture::LoadRaw(int Width, int Height, int BPP, unsigned char* pData)
{
    m_imageWidth = Width;
    m_imageHeight = Height;
    m_imageBPP = BPP;

    return LoadInternal(pData);
}


bool Texture::LoadInternal(void* image_data)
{
    glGenTextures(1, &m_ID);
    glBindTexture(m_textureTarget, m_ID);

    if (m_textureTarget == GL_TEXTURE_2D) {
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
    } else {
        DEV_ERROR("Support for texture target %x is not implemented\n", m_textureTarget);
        return false;
    }

    glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(m_textureTarget);

    glBindTexture(m_textureTarget, 0);

    return true;
}

void Texture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_ID);
}
