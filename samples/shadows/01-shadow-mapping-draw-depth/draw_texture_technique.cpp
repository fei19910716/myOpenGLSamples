
#include "draw_texture_technique.h"

#include "base/utils.h"

DrawTextureTechnique::DrawTextureTechnique()
{
}

bool DrawTextureTechnique::Init()
{
    if (!Technique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, "shaders/shadows/draw_texture.vs")) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, "shaders/shadows/draw_texture.fs")) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_textureLocation = GetUniformLocation("gSampler");

    if (m_textureLocation == INVALID_UNIFORM_LOCATION) {
        return false;
    }

    return true;
}

void DrawTextureTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_textureLocation, TextureUnit);
}