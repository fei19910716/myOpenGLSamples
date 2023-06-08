
#pragma once

#include "base/technique.h"
#include "base/math_3d.h"

class DisplayScreenTechnique : public Technique {

public:

    DisplayScreenTechnique();

    virtual bool Init();

    void SetTextureUnit(unsigned int TextureUnit);
private:

    GLuint m_textureLocation;
};