
#include "display_screen_technique.h"

#include "base/utils.h"

DisplayScreenTechnique::DisplayScreenTechnique()
{
}

bool DisplayScreenTechnique::Init()
{
    if (!Technique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, "shaders/display_screen.vs")) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, "shaders/display_screen.fs")) {
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

void DisplayScreenTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_textureLocation, TextureUnit);
}