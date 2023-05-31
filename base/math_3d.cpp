
#include "math_3d.h"

inline Vector3f::Vector3f(const Vector4f& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
}