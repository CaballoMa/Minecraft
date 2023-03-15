#ifndef NOISE_H
#define NOISE_H

#include "glm_includes.h"
#include <cmath>
#include <math.h>

class Noise {
public:
    static int calculateHeight(int x, int z, float thresHold);
    static float grasslands(float x, float z);
    static float mountains(float x, float z);

    static float worleyNoise(float x, float z);
    //float rand(glm::vec2 p);
    static glm::vec2 random2(glm::vec2 p);
\
    static float fbm2D(float x, float z, float persistence);
    static float perlinNoise2D(glm::vec2 uv);
    static float surflet(glm::vec2 p, glm::vec2 gridPoint);
    static glm::vec2 pow(glm::vec2 v, int p);
    static glm::vec2 noise2DNormalVector(glm::vec2 v);

    static float interpNoise2D(float x, float z);
    static float smoothNoise2D(float x, float z);
    static float noise2D(float x, float z);

    static float cosineInterp(float a, float b, float t);
    static float linearInterp(float a, float b, float t);

    static float powNum(float f, int p);
    static glm::vec2 powVec2(glm::vec2 v, int p);
    static glm::vec3 powVec3(glm::vec3 v, int p);

    static float perlinNoise3D(glm::vec3 p);
    static float surflet(glm::vec3 p, glm::vec3 gridPoint);
    static glm::vec3 random3(glm::vec3 p);

    static glm::vec2 rotate(glm::vec2 p, float deg);
};

#endif // NOISE_H
