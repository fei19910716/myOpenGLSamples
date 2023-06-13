
#pragma once

#include <string>

#include <glad/glad.h>

class Texture
{
public:
    Texture(GLenum TextureTarget, const std::string& FileName);

    ~Texture();

    bool Load(unsigned int BufferSize, void* pData);

    bool Load(const std::string& Filename);

    bool LoadRaw(int Width, int Height, int BPP, unsigned char* pData);

    // Must be called at least once for the specific texture unit
    void Bind(GLenum TextureUnit);

    void GetImageSize(int& ImageWidth, int& ImageHeight)
    {
        ImageWidth = m_imageWidth;
        ImageHeight = m_imageHeight;
    }

    GLuint GetTexture() const { return m_textureObj; }

private:
    bool LoadInternal(void* image_data);

    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageBPP = 0;
};
