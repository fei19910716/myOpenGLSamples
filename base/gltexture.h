
#pragma once

#include <string>

#include <glad/glad.h>

enum TextureType{
    General,
    DiffuseMap,
    SpeculerMap,
    HeightMap,
    NormalMap
};

class GLTexture
{
public:
    GLTexture(GLenum TextureTarget, const std::string& FileName, TextureType type = TextureType::General);

    ~GLTexture();

    bool Load(unsigned int BufferSize, void* pData);

    bool Load(const std::string& Filename);

    bool LoadRaw(int Width, int Height, int BPP, unsigned char* pData);

    // Must be called at least once for the specific texture unit
    bool Bind(GLenum TextureUnit) const;

    inline void GetImageSize(int& ImageWidth, int& ImageHeight)
    {
        ImageWidth = m_imageWidth;
        ImageHeight = m_imageHeight;
    }

    inline GLuint      GetTextureID()   const { return m_ID; }
    inline TextureType GetTextureType() const { return m_textureType; }
    inline std::string GetFilePath()    const { return m_filePath; }

private:
    bool LoadInternal(void* image_data);

    std::string m_filePath;
    TextureType m_textureType;
    GLenum m_textureTarget;
    GLuint m_ID;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageBPP = 0;
};
