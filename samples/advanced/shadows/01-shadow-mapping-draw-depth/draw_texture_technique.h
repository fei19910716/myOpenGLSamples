
#pragma once

#include "base/technique.h"
#include "base/math_3d.h"

class DrawTextureTechnique : public Technique {

public:

    DrawTextureTechnique();

    virtual bool Init();

    void SetTextureUnit(unsigned int TextureUnit);
private:

    GLuint m_textureLocation;
};