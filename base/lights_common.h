
#pragma once

#include <string>

#include "base/math.h"


class BaseLight
{
public:
    BaseLight()
    {
        Color = glm::vec3(0.0f, 0.0f, 0.0f);
        AmbientIntensity = 0.0f;
        DiffuseIntensity = 0.0f;
        SpeculerIntensity = 0.0f;
    }

    virtual ~BaseLight() = default;

public:
    std::string Name;
    glm::vec3   Color;
    float       AmbientIntensity;
    float       DiffuseIntensity;
    float       SpeculerIntensity;
};


class DirectionalLight : public BaseLight
{      
public:
    glm::vec3 Direction;

    DirectionalLight() : BaseLight()
    {
        Direction = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};


class PointLight : public BaseLight
{
public:

    struct LightAttenuation
    {
        float Constant;
        float Linear;
        float Exp;
        
        LightAttenuation()
        {
            Constant = 1.0f;
            Linear = 0.0f;
            Exp = 0.0f;
        }
    };

    PointLight(): BaseLight()
    {
        Position = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    glm::vec3 Position;
    LightAttenuation Attenuation;
};


class SpotLight : public PointLight
{
public:
    glm::vec3 Direction;
    float Cutoff;
    float OuterCutOff; // for soft edges

    SpotLight(): PointLight()
    {
        Direction = glm::vec3(0.0f, 0.0f, 0.0f);
        Cutoff = 0.0f;
        OuterCutOff = 0.0f;
    }
};


#define COLOR_WHITE Vector3f(1.0f, 1.0f, 1.0f)
#define COLOR_RED   Vector3f(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN Vector3f(0.0f, 1.0f, 0.0f)
#define COLOR_CYAN  Vector3f(0.0f, 1.0f, 1.0f)
#define COLOR_BLUE  Vector3f(0.0f, 0.0f, 1.0f)
