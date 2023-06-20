
#pragma once

#include <string>
#include <vector>
#include <cassert>

#include "base/globject.h"

enum TextureType{
    General,
    DiffuseMap,
    SpeculerMap,
    HeightMap,
    NormalMap
};

class GLTexture: public GLObject
{
public:
    GLTexture() = default;
    GLTexture(GLenum TextureTarget, const std::vector<std::string>& files, TextureType type = TextureType::General);
    GLTexture(GLenum TextureTarget, const std::string& file, TextureType type = TextureType::General);

    ~GLTexture();

    bool LoadTexture2D(unsigned int BufferSize, void* pData);

    bool LoadTexture2D(const std::string& Filename);

    bool LoadTexture2D(int Width, int Height, int BPP, unsigned char* pData);

    bool LoadCubemap(const std::vector<std::string>& faces);

    // Must be called at least once for the specific texture unit
    bool Bind(GLenum TextureUnit) const;

    inline void GetImageSize(int& ImageWidth, int& ImageHeight)
    {
        ImageWidth = m_imageWidth;
        ImageHeight = m_imageHeight;
    }

    inline TextureType GetTextureType() const { return m_textureType; }
    inline std::string GetFilePath()    const { 
        assert(m_textureTarget == GL_TEXTURE_2D);

        return m_texture2DPath; 
    }

private:
    bool LoadInternal(void* image_data);

    std::string m_texture2DPath;
    TextureType m_textureType;
    GLenum m_textureTarget;

    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageBPP = 0;
};
