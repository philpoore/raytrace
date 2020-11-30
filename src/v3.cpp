#include "ray.h"

v3 V3(f32 x, f32 y, f32 z)
{
    v3 a;
    a.x = x;
    a.y = y;
    a.z = z;
    return a;
}

v3 add(v3 a, v3 b)
{
    v3 c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
}

v3 subtract(v3 a, v3 b)
{
    v3 c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return c;
}

v3 multiply(v3 a, v3 b)
{
    v3 c;
    c.x = a.x * b.x;
    c.y = a.y * b.y;
    c.z = a.z * b.z;
    return c;
}

v3 multiply(f32 a, v3 b)
{
    v3 c;
    c.x = a * b.x;
    c.y = a * b.y;
    c.z = a * b.z;
    return c;
}

v3 divide(v3 a, v3 b)
{
    v3 c;
    c.x = a.x / b.x;
    c.y = a.y / b.y;
    c.z = a.z / b.z;
    return c;
}

v3 divide(v3 a, f32 b)
{
    v3 c;
    c.x = a.x / b;
    c.y = a.y / b;
    c.z = a.z / b;
    return c;
}

f32 dot(v3 a, v3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

// normalise or zero
v3 noz(v3 a)
{
    v3 b;
    f32 mag = sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
    b.x = a.x / mag;
    b.y = a.y / mag;
    b.z = a.z / mag;
    return b;
}

// ??
v3 cross(v3 a, v3 b)
{
    v3 c;
    c.x = (a.y * b.z) - (a.z * b.y);
    c.y = (a.z * b.x) - (a.x * b.z);
    c.z = (a.x * b.y) - (a.y * b.x);
    return c;
}


v3 lerp(v3 a, v3 b, f32 f)
{
    v3 c;
    c.x = (b.x - a.x) * f + a.x;
    c.y = (b.y - a.y) * f + a.y;
    c.z = (b.z - a.z) * f + a.z;
    return c;
}
