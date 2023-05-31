#pragma once

#ifdef _WIN64
#ifndef _USE_MATH_DEFINES
// For some reason this works on in Debug build of Visual Studio but not in Release build.
// The solution people suggested was to define this as a preprocessor macro in the project.
#define _USE_MATH_DEFINES 
#endif
#include <cmath>
#else
#include <math.h>
#endif
#include <stdio.h>
#include <cfloat>


struct Vector2i
{
    int x;
    int y;
};

struct Vector3i
{
    union {
        float x;
        float r;
    };

    union {
        float y;
        float g;
    };

    union {
        float z;
        float b;
    };
};

struct Vector2f
{
    union {
        float x;
        float u;
    };

    union {
        float y;
        float v;
    };

    Vector2f()
    {
    }

    Vector2f(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
};

struct Vector4f;
struct Vector3f
{
    union {
        float x;
        float r;
    };

    union {
        float y;
        float g;
    };

    union {
        float z;
        float b;
    };

    Vector3f() {}

    Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f(const float* pFloat)
    {
        x = pFloat[0];
        y = pFloat[1];
        z = pFloat[2];
    }

    Vector3f(float f)
    {
        x = y = z = f;
    }

    Vector3f(const Vector4f& v);

    Vector3f& operator+=(const Vector3f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;

        return *this;
    }

    Vector3f& operator-=(const Vector3f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;

        return *this;
    }

    Vector3f& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }

    bool operator==(const Vector3f& r)
    {
        return ((x == r.x) && (y == r.y) && (z == r.z));
    }

    bool operator!=(const Vector3f& r)
    {
        return !(*this == r);
    }

    operator const float*() const
    {
        return &(x);
    }


    Vector3f Cross(const Vector3f& v) const;

    float Dot(const Vector3f& v) const
    {
        float ret = x * v.x + y * v.y + z * v.z;
        return ret;
    }

    float Distance(const Vector3f& v) const
    {
        float delta_x = x - v.x;
        float delta_y = y - v.y;
        float delta_z = z - v.z;
        float distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
        return distance;
    }

    float Length() const
    {
        float len = sqrtf(x * x + y * y + z * z);
        return len;
    }

    bool IsZero() const
    {
        return ((x + y + z) == 0.0f);
    }

    Vector3f& Normalize();

    void Rotate(float Angle, const Vector3f& Axis);

    Vector3f Negate() const;

    void Print(bool endl = true) const
    {
        printf("(%f, %f, %f)", x, y, z);

        if (endl) {
            printf("\n");
        }
    }
};

struct Vector4f
{
    float x;
    float y;
    float z;
    float w;

    Vector4f()
    {
    }

    Vector4f(float _x, float _y, float _z, float _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }

    Vector4f(const Vector3f& v, float _w)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = _w;
    }

    void Print(bool endl = true) const
    {
        printf("(%f, %f, %f, %f)", x, y, z, w);

        if (endl) {
            printf("\n");
        }
    }

    Vector3f to3f() const
    {
        Vector3f v(x, y, z);
        return v;
    }

    float Length() const
    {
        float len = sqrtf(x * x + y * y + z * z + w * w);
        return len;
    }

    Vector4f& Normalize();

    float Dot(const Vector4f& v) const
    {
        float ret = x * v.x + y * v.y + z * v.z + w * v.w;
        return ret;
    }
};


inline Vector2f operator*(const Vector2f& l, float f)
{
    Vector2f Ret(l.x * f, l.y * f);

    return Ret;
}


inline Vector3f operator+(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x + r.x,
                 l.y + r.y,
                 l.z + r.z);

    return Ret;
}

inline Vector3f operator-(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x - r.x,
                 l.y - r.y,
                 l.z - r.z);

    return Ret;
}

inline Vector3f operator*(const Vector3f& l, float f)
{
    Vector3f Ret(l.x * f,
                 l.y * f,
                 l.z * f);

    return Ret;
}

inline Vector3f operator/(const Vector3f& l, float f)
{
    Vector3f Ret(l.x / f,
                 l.y / f,
                 l.z / f);

    return Ret;
}



inline Vector4f operator+(const Vector4f& l, const Vector4f& r)
{
    Vector4f Ret(l.x + r.x,
                 l.y + r.y,
                 l.z + r.z,
                 l.w + r.w);

    return Ret;
}

inline Vector4f operator/(const Vector4f& l, float f)
{
    Vector4f Ret(l.x / f,
                 l.y / f,
                 l.z / f,
                 l.w / f);

    return Ret;
}

inline Vector4f operator*(const Vector4f& l, float f)
{
    Vector4f Ret(l.x * f,
                 l.y * f,
                 l.z * f,
                 l.w * f);

    return Ret;
}

inline Vector4f operator*(float f, const Vector4f& l)
{
    Vector4f Ret(l.x * f,
                 l.y * f,
                 l.z * f,
                 l.w * f);

    return Ret;
}