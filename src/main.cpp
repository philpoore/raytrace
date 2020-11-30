#include "ray.h"

Image newImage(u32 width, u32 height) {
    Image myImage;

    u32 numPixels = sizeof(u32) * width * height;
    u32 *pixels = (u32 *)malloc(numPixels);

    myImage.width = width;
    myImage.height = height;
    myImage.pixels = pixels;

    return myImage;
}

void writeImage(Image image, char *filename) {
    u32 sizeImage = image.width * image.height * sizeof(32);
    BitmapHeader bitmapHeader = {};

    bitmapHeader.FileType = 0x4D42;
    bitmapHeader.FileSize = sizeof(bitmapHeader) + sizeImage;
    bitmapHeader.BitmapOffset = sizeof(bitmapHeader);
    bitmapHeader.Size = sizeof(bitmapHeader) - 14;
    bitmapHeader.Width = image.width;
    bitmapHeader.Height = image.height;
    bitmapHeader.Planes = 1;
    bitmapHeader.BitsPerPixel = 32;
    bitmapHeader.Compression = 0;
    bitmapHeader.SizeOfBitmap = sizeImage;
    bitmapHeader.HozResolution = 0;
    bitmapHeader.VertResolution = 0;
    bitmapHeader.ColorsUsed = 0;
    bitmapHeader.ColorsImportant = 0;

    FILE *file = fopen(filename, "wb");
    if (file) {
        fwrite(&bitmapHeader, sizeof(bitmapHeader), 1, file);
        fwrite(image.pixels, sizeImage, 1, file);
        fclose(file);
    }
}

f32 clamp(f32 a, f32 b) {
    if (a < 0) {
        return 0;
    } else if (a > b) {
        return b;
    } else {
        return a;
    }
}

#define ratio 1.0 / 4.0
v3 gammaCorrect(v3 color) {
    v3 b = {};
    b.x = pow(color.x, ratio);
    b.y = pow(color.y, ratio);
    b.z = pow(color.z, ratio);

    return b;
}

u32 v3toRGBpacked(v3 color) {
    u32 result = (((u32)clamp((255.0 * color.x), 255.0) << 16) +
                  ((u32)clamp((255.0 * color.y), 255.0) << 8) +
                  ((u32)clamp((255.0 * color.z), 255.0)));

    return result;
}

f32 randLateral() { return (f32)rand() / (f32)RAND_MAX; }

f32 randBiLateral() { return 2.0 * randLateral() - 1.0; }

v3 randomSample() {
    v3 a;
    a.x = randBiLateral();
    a.y = randBiLateral();
    a.z = randBiLateral();
    return a;
}

v3 rayCast(World *world, v3 rayO, v3 rayD) {
    // printf("rayO %g %g %g\n", rayO.x, rayO.y, rayO.z);
    // printf("rayD %g %g %g\n", rayD.x, rayD.y, rayD.z);

    // v3 color = V3(0, 0, 0);
    v3 result = {};
    v3 attenuation = V3(1.0, 1.0, 1.0);

    f32 tolerance = 0.00001f;

    for (u32 rayCount = 0; rayCount < 16; rayCount++) {

        u32 hitMatIndex = 0;
        f32 minDistance = 0.001f;
        f32 hitDistance = f32Max;
        v3 nextNormal = {};
        v3 nextOrigin = {};
        // printf("hitDistance %g\n", hitDistance);

        for (u32 i = 0; i < world->planeCount; i++) {
            Plane plane = world->planes[i];

            // plane intersects line
            f32 denom = dot(plane.N, rayD);

            // exit(1);
            if ((denom < -tolerance) || (denom > tolerance)) {
                f32 t = (-plane.d - dot(plane.N, rayO)) / denom;
                if (t > minDistance && t < hitDistance) {
                    // printf("denom %d, %g %g\n", i, denom, distance);
                    hitDistance = t;
                    hitMatIndex = plane.matIndex;

                    nextOrigin = add(rayO, multiply(t, rayD));
                    nextNormal = plane.N;
                }
            }
        }

        for (u32 i = 0; i < world->sphereCount; i++) {
            Sphere sphere = world->spheres[i];

            v3 sphereLocalOrigin = subtract(rayO, sphere.P);
            // plane intersects line

            f32 a = dot(rayD, rayD);
            f32 b = 2.0 * dot(sphereLocalOrigin, rayD);
            f32 c =
                dot(sphereLocalOrigin, sphereLocalOrigin) - sphere.r * sphere.r;

            f32 denom = 2.0 * a;
            f32 rootTerm = sqrt(b * b - 4.0 * a * c);
            if (rootTerm > tolerance) {
                f32 tp = (-b + rootTerm) / denom;
                f32 tn = (-b - rootTerm) / denom;

                f32 t = tp;
                if (tn > minDistance && tn < tp) {
                    t = tn;
                }

                if (t > minDistance && t < hitDistance) {
                    // printf("denom %d, %g %g\n", i, denom, t);
                    hitDistance = t;
                    hitMatIndex = sphere.matIndex;

                    nextOrigin = add(rayO, multiply(t, rayD));
                    nextNormal = noz(subtract(nextOrigin, sphere.P));
                }
            }
        }

        if (hitMatIndex) {
            Material mat = world->materials[hitMatIndex];

            // result = mat.refColor;
            result = add(result, multiply(attenuation, mat.emitColor));

            // f32 cosAtten = 1.0;
            f32 cosAtten = dot(subtract(V3(0, 0, 0), rayD), nextNormal);
            if (cosAtten < 0) {
                cosAtten = 0;
            }

            // printf("%d: %f %f %f\n", hitMatIndex, attenuation.x,
            // attenuation.y, attenuation.z);
            attenuation =
                multiply(attenuation, multiply(cosAtten, mat.refColor));

            rayO = nextOrigin;

            // reflection
            v3 pureBounce = subtract(
                rayD, multiply(2.0 * dot(rayD, nextNormal), nextNormal));

            v3 randomBounce = noz(add(nextNormal, randomSample()));

            rayD = noz(lerp(randomBounce, pureBounce, mat.scatter));
        } else {
            Material mat = world->materials[0];
            result = add(result, multiply(attenuation, mat.emitColor));

            break;
        }
    }
    // exit(1);
    return result;
}

f32 LinearToSRGB(f32 L) {
    if (L < 0.0) {
        // printf(".");
        L = 0.0;
    }
    if (L > 1.0) {
        // printf("x");
        L = 1.0;
    }
    // printf("o");

    f32 S = L * 12.92;
    if (L >= 0.0031308) {
        S = 1.055 * (f32)pow(L, 0.41666666) - 0.55;
    }
    return S;
}

int main(int argc, char **argv) {
    char *outputImageFilename = "image.bmp";

    if (argc >= 2) {
        outputImageFilename = argv[1];
    }

    printf("raytrace - v0.0.1\n");

    Material materials[7] = {};
    materials[0].emitColor = v3{0.8, 0.8, 1.0};

    materials[1].refColor = v3{0.5, 0.5, 0.5};

    materials[2].refColor = v3{0.7, 0.5, 0.3};
    // materials[2].scatter = 1.0;
    // materials[3].emitColor = v3{2.0, 0.0, 0.0};
    materials[3].emitColor = v3{2.0, 0.0, 0.0};
    // materials[3].scatter = 1.0;

    materials[4].refColor = v3{0, 1.0, 0};
    materials[4].scatter = 1.0;

    materials[5].refColor = v3{1.0, 0.0, 0.0};
    materials[5].scatter = 0.9;

    materials[6].refColor = v3{0.98, 0.98, 0.98};
    materials[6].scatter = 1.0;

    // materials[4].refColor = v3{0.4, 0.8, 0.2};
    // materials[4].scatter = 0.7;

    Plane planes[1] = {};
    planes[0].N = v3{0, 0, 1};
    planes[0].d = 0;
    planes[0].matIndex = 1;

    Sphere spheres[5] = {};
    spheres[0].P = v3{0, 0, 0};
    spheres[0].r = 1;
    spheres[0].matIndex = 2;

    spheres[1].P = v3{3, -2, 0};
    spheres[1].r = 1;
    spheres[1].matIndex = 3;

    spheres[2].P = v3{-2, -1, 2};
    spheres[2].r = 1;
    spheres[2].matIndex = 4;

    spheres[3].P = v3{1, -2, 2};
    spheres[3].r = 1.0;
    spheres[3].matIndex = 5;

    spheres[4].P = v3{-2, 2, 0};
    spheres[4].r = 2.0;
    spheres[4].matIndex = 6;

    // planes[1].N = v3{1, 0, 0};
    // planes[1].d = 0;
    // planes[1].matIndex = 2;
    //
    // planes[2].N = v3{1, 1, 0};
    // planes[2].d = 0;
    // planes[2].matIndex = 3;

    World world = {};
    world.materialCount = 7;
    world.materials = materials;

    world.planeCount = 1;
    world.planes = planes;

    world.sphereCount = 5;
    world.spheres = spheres;

    u32 outputWidth = 1600;
    u32 outputHeight = 1200;
    Image image = newImage(outputWidth, outputHeight);

    v3 cameraP = v3{0, -10, 1};
    v3 cameraZ = noz(cameraP);
    v3 cameraX = noz(cross(V3(0, 0, 1), cameraZ));
    v3 cameraY = noz(cross(cameraZ, cameraX));

    f32 filmDist = 1.0;
    f32 filmW = 1.0;
    f32 filmH = 1.0;

    if (image.width > image.height) {
        filmH = filmW * ((f32)image.height / (f32)image.width);
    } else if (image.height > image.width) {
        filmW = filmH * ((f32)image.width / (f32)image.height);
    }

    f32 halfFilmW = filmW * 0.5;
    f32 halfFilmH = filmH * 0.5;

    f32 halfPixelW = 0.5 / image.width;
    f32 halfPixelH = 0.5 / image.height;

    u32 raysPerPixel = 32;

    v3 filmCenter = subtract(cameraP, multiply(filmDist, cameraZ));

    u32 *Out = image.pixels;
    for (u32 y = 0; y < image.height; y++) {
        f32 filmY = -1.0 + 2.0 * (((f32)y) / image.height);
        for (u32 x = 0; x < image.width; x++) {
            f32 filmX = -1.0 + 2.0 * (((f32)x) / image.width);

            v3 color = {};
            for (int rayIndex = 0; rayIndex < raysPerPixel; rayIndex++) {
                f32 offX = filmX + randBiLateral() * halfPixelW;
                f32 offY = filmY + randBiLateral() * halfPixelH;

                v3 filmP =
                    add(filmCenter, add(multiply(offY * halfFilmH, cameraY),
                                        multiply(offX * halfFilmW, cameraX)));

                v3 rayO = cameraP;
                v3 rayD = noz(subtract(filmP, cameraP));

                v3 contrib = divide(rayCast(&world, rayO, rayD), raysPerPixel);
                color = add(contrib, color);
            }

            // color.x = (f32)x / image.width;
            // color.y = (f32)y / image.height;
            // color.z = 0.0;

            // if (color.x > 1.0 || color.x < 0.0){
            //     exit(1);
            // }

            color = gammaCorrect(color);

            v3 srgbColor;
            srgbColor.x = LinearToSRGB(color.x);
            srgbColor.y = LinearToSRGB(color.y);
            srgbColor.z = LinearToSRGB(color.z);

            // printf("%f %f %f\n", color.x, color.y, color.z);
            // printf("%e %e %e\n", srgbColor.x, srgbColor.y, srgbColor.z);

            u32 finalColor = v3toRGBpacked(srgbColor);

            // if (x < image.width / 2.0){
            //     finalColor = v3toRGBpacked(srgbColor);
            // }
            // u32 offset = x + (y * OutputWidth);
            *Out++ = finalColor;

            // exit(1);
        }
    }

    printf("Saving output to : %s\n", outputImageFilename);
    writeImage(image, outputImageFilename);

    return 0;
}
