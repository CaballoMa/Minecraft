#include "noise.h"

const float PI = 3.141593;

int Noise::calculateHeight(int x, int z, float thresHold)
{
    float grass = grasslands(x, z);
    float mtn = mountains(x, z);

    // float f1 = glm::smoothstep(0.4, 0.5, (double)(fbm2D(x/2048.f, z/2048.f, 0.1)));

    float f1 = glm::smoothstep(0.25, 0.75, (double)thresHold);
    return glm::mix(grass, mtn, f1);
}

float Noise::grasslands(float x, float z)
{
    x /= 512;
    z /= 512;

    int min = 128;
    int max = 145;

    float x1 = fbm2D(x, z, 1.1);
    glm::vec2 v1 = rotate(glm::vec2(x, z), 10.f);
    float z1 = fbm2D(v1[0], v1[1], 1.1);

    float theta = glm::clamp(worleyNoise(x1, z1), 0.f, 1.f);

    return min + (max - min) * theta;
}

float Noise::mountains(float x, float z)
{
    x /= 256;
    z /= 256;

    int min = 145;
    int max = 255;

    //float theta = worleyNoise(x, z);
    float theta = glm::clamp(1 - worleyNoise(x, z), 0.f, 1.f);

    return min + (max - min) * theta;
}

float Noise::worleyNoise(float x, float z)
{
    double intX, fractX;
    fractX = modf(x, &intX);

    double intZ, fractZ;
    fractZ = modf(z, &intZ);

    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(glm::vec2(intX, intZ) + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            glm::vec2 diff = neighbor + point - glm::vec2(fractX, fractZ); // Distance between fragment coord and neighbor’s Voronoi point
            float dist = glm::length(diff);
            if (dist < minDist)
            {
                minDist = dist;
            }

            // minDist = min(minDist, dist);
        }
    }
    return minDist;
}

glm::vec2 Noise::random2(glm::vec2 p)
{
    float x = glm::fract(glm::sin(glm::dot(p, glm::vec2(127.1, 311.7))) * 43758.5453);
    float z = glm::fract(glm::sin(glm::dot(p, glm::vec2(269.5,183.3))) * 43758.5453);
    return glm::vec2(x, z);
}

float Noise::powNum(float f, int p)
{
    for (int i = 0; i < p; i++)
    {
        f *= f;
    }
    return f;
}

glm::vec2 Noise::powVec2(glm::vec2 v, int p)
{
    for (int i = 0; i < p; i++)
    {
        v *= v;
    }

    return v;
}

float Noise::fbm2D(float x, float z, float persistence)
{
    float total = 0;
    int octaves = 4;
    float frequency = 3.f;
    float amplitude = 0.7;

    for (int i = 0; i < octaves; i++) {
        total += interpNoise2D(x * frequency, z * frequency) * amplitude;

        frequency *= 2.f;
        amplitude *= persistence;
    }

    return (total + 1) / 2;
}

float Noise::surflet(glm::vec2 P, glm::vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = glm::abs(P[0] - gridPoint[0]);
    float distY = glm::abs(P[0] - gridPoint[0]);
    float tX = 1 - 6 * powNum(distX, 5) + 15 * powNum(distX, 4) - 10 * powNum(distX, 3);
    float tY = 1 - 6 * powNum(distY, 5) + 15 * powNum(distY, 4) - 10 * powNum(distY, 3);
    // Get the random vector for the grid point
    glm::vec2 gradient = 2.f * random2(gridPoint) - glm::vec2(1.f);
    // Get the vector from the grid point to P
    glm::vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

float Noise::perlinNoise2D(glm::vec2 uv)
{
    float surfletSum = 0.f;

    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = 0; dy <= 1; ++dy) {
            surfletSum += surflet(uv, glm::floor(uv) + glm::vec2(dx, dy));
        }
    }

    return surfletSum;
}

/*
float Noise::surflet(glm::vec2 p, glm::vec2 gridPoint)
{
    glm::vec2 t2 = glm::abs(p - gridPoint);
    glm::vec2 t = glm::vec2(1.f) - 6.f * powVec2(t2, 5) + 15.f * powVec2(t2, 4) - 10.f * powVec2(t2, 3);
    glm::vec2 gradient = noise2DNormalVector(gridPoint) * 2.f - glm::vec2(1,1);
    glm::vec2 diff = p - gridPoint;

    float height = glm::dot(diff, gradient);

    return height * t[0] * t[1];
}
*/

glm::vec2 Noise::noise2DNormalVector(glm::vec2 v)
{
    v += 0.01;
    glm::mat2 primes = glm::mat2{{126.1, 311.7}, {420.2, 1337.1}};

    return glm::normalize(glm::abs(glm::fract(glm::sin(primes * v))));
}

float Noise::interpNoise2D(float x, float z)
{
    double intX, fractX;
    fractX = modf(x, &intX);

    double intZ, fractZ;
    fractZ = modf(z, &intZ);

    float v1 = smoothNoise2D(intX, intZ);
    float v2 = smoothNoise2D(intX + 1, intZ);
    float v3 = smoothNoise2D(intX, intZ + 1);
    float v4 = smoothNoise2D(intX + 1, intZ + 1);

    float i1 = cosineInterp(v1, v2, fractX);
    float i2 = cosineInterp(v3, v4, fractX);

    return cosineInterp(i1, i2, fractZ);
}
float Noise::smoothNoise2D(float x, float z)
{
    float corners = (noise2D(x - 1, z - 1) +
                     noise2D(x + 1, z - 1) +
                     noise2D(x - 1, z + 1) +
                     noise2D(x + 1, z + 1)) / 16;
    float sides = (noise2D(x - 1, z) +
                   noise2D(x + 1, z) +
                   noise2D(x, z - 1) +
                   noise2D(x, z + 1)) / 8;
    float center = noise2D(x, z) / 4;

    return corners + sides + center;
}
float Noise::noise2D(float x, float z)
{
    float s = sin(glm::dot(glm::vec2(x, z), glm::vec2(127.1, 311.7))) * 43758.5453;

    return modf(s, nullptr);
}

float Noise::cosineInterp(float a, float b, float t)
{
    t = (1 - cos(t * PI)) / 2.f;

    return linearInterp(a, b, t);
}
float Noise::linearInterp(float a, float b, float t)
{
    return a * (1 - t) + b * t;
}

glm::vec3 Noise::powVec3(glm::vec3 v, int p)
{
    for (int i = 0; i < p; i++)
    {
        v *= v;
    }
    return v;
}

float Noise::perlinNoise3D(glm::vec3 p)
{
    float surfletSum = 0.f;
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            for(int dz = 0; dz <= 1; ++dz) {
                surfletSum += surflet(p, glm::floor(p) + glm::vec3(dx, dy, dz));
            }
        }
    }
    return surfletSum;
}

float Noise::surflet(glm::vec3 p, glm::vec3 gridPoint)
{
    glm::vec3 t2 = glm::abs(p - gridPoint);
    glm::vec3 t = glm::vec3(1.f) - 6.f * powVec3(t2, 5.f) + 15.f * powVec3(t2, 4.f) - 10.f * powVec3(t2, 3.f);
    glm::vec3 gradient = random3(gridPoint) * 2.f - glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 diff = p - gridPoint;

    float height = glm::dot(diff, gradient);

    return height * t.x * t.y * t.z;
}

glm::vec3 Noise::random3(glm::vec3 p)
{
    float x = glm::fract(glm::sin(glm::dot(p, glm::vec3(127.1, 311.7, 275.2))) * 43758.5453);
    float y = glm::fract(glm::sin(glm::dot(p, glm::vec3(269.5, 183.3, 167.7))) * 43758.5453);
    float z = glm::fract(glm::sin(glm::dot(p, glm::vec3(420.6, 631.2, 728.1))) * 43758.5453);
    return glm::vec3(x, y, z);
}

glm::vec2 Noise::rotate(glm::vec2 p, float deg)
{
    float rad = deg * 3.14159 / 180.0;
    return glm::vec2(cos(rad) * p.x - sin(rad) * p.y,
                sin(rad) * p.x + cos(rad) * p.y);
}
