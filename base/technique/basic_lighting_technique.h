
#ifndef LIGHTING_TECHNIQUE_H
#define LIGHTING_TECHNIQUE_H

#include "technique.h"

#include "base/lights_common.h"

// This is the old version of the lighting technique which was used for the website before the youtube channel
// For the newer version see new_lighting.h

class BasicLightingTechnique : public Technique {
public:

    static const unsigned int MAX_POINT_LIGHTS = 2;
    static const unsigned int MAX_SPOT_LIGHTS = 2;

    BasicLightingTechnique();

    virtual bool Init();

    void SetWVP(const Matrix4f& WVP);
    void SetWorldMatrix(const Matrix4f& WVP);
    void SetColorTextureUnit(unsigned int TextureUnit);
    void SetDirectionalLight(const DirectionalLight& Light);
    void SetPointLights(unsigned int NumLights, const PointLight* pLights);
    void SetSpotLights(unsigned int NumLights, const SpotLight* pLights);
    void SetEyeWorldPos(const Vector3f& EyeWorldPos);
    void SetMatSpecularIntensity(float Intensity);
    void SetMatSpecularPower(float Power);
    void SetColorMod(const Vector4f& ColorMod);

private:

    GLuint m_WVPLocation;
    GLuint m_WorldMatrixLocation;
    GLuint m_colorTextureLocation;
    GLuint m_eyeWorldPosLocation;
    GLuint m_matSpecularIntensityLocation;
    GLuint m_matSpecularPowerLocation;
    GLuint m_numPointLightsLocation;
    GLuint m_numSpotLightsLocation;
    GLuint m_colorModLocation;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Direction;
    } m_dirLightLocation;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } m_pointLightsLocation[MAX_POINT_LIGHTS];

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        GLuint Direction;
        GLuint Cutoff;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } m_spotLightsLocation[MAX_SPOT_LIGHTS];
};


#endif  /* LIGHTING_TECHNIQUE_H */