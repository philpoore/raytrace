#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <float.h>

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;

typedef char s8;
typedef short s16;
typedef int s32;

typedef float f32;

#define f32Max FLT_MAX

#include "v3.h"


#pragma pack(push, 1)
struct BitmapHeader {
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;
    u32 Size;
    s32 Width;
    s32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 Compression;
    u32 SizeOfBitmap;
    s32 HozResolution;
    s32 VertResolution;
    u32 ColorsUsed;
    u32 ColorsImportant;
};
#pragma pack(pop)

struct Image
{
    u32 width;
    u32 height;
    u32 *pixels;
};

struct Material
{
    f32 scatter;
    v3 emitColor;
    v3 refColor;
};

struct Plane
{
    v3 N;
    f32 d;
    u32 matIndex;
};

struct Sphere
{
    v3 P;
    f32 r;
    u32 matIndex;
};

struct World
{
    u32 materialCount;
    Material *materials;

    u32 planeCount;
    Plane *planes;

    u32 sphereCount;
    Sphere *spheres;
};
