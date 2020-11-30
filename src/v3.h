#include <math.h>

struct v3 {
    f32 x;
    f32 y;
    f32 z;
};

v3 V3(f32 x, f32 y, f32 z);

v3 add(v3 a, v3 b);
v3 subtract(v3 a, v3 b);
v3 multiply(v3 a, v3 b);
v3 multiply(f32 a, v3 b);
v3 divide(v3 a, v3 b);
v3 divide(v3 a, f32 b);
f32 dot(v3 a, v3 b);
v3 noz(v3 a);
v3 cross(v3 a, v3 b);
v3 lerp(v3 a, v3 b, f32 f);
