#version 330
in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_sampler;

uniform int u_Time;

uniform int u_postType;

uniform ivec2 u_Dimensions;

// Time coefficient used to modify random function, final displacement of UV, and color mix
float coeff = 0.156 * cos(u_Time / 360.f) + 0.643 * sin(u_Time / 180.f) +
           0.733 * cos(u_Time / 240.f) + 0.239 * sin(u_Time / 90.f);


// Perlin noise implementation as per Noise Functions slide deck

vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(623.3, 31.3)),
                          dot(p, vec2(724.1, 58.9))))
                 * 32512.2372 + coeff);
}
float surflet(vec2 p , vec2 gridPoint) {
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    vec2 diff = abs(p - gridPoint);
    vec2 t = vec2(1.f) - vec2(6.f) * pow(diff, vec2(5.f)) + vec2(15.f) * pow(diff, vec2(4.f)) - vec2(10.f) * pow(diff, vec2(3.f));

    // Get the random vector for the grid point (assume we wrote a function random2)
    vec2 gradient = random2(gridPoint);

    // Get the vector from the grid point to P
    vec2 diff2 = p - gridPoint;

    // Get the value of our height field by dotting grid->P with our gradient
    float height = dot(diff2, gradient);

    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y;

}

float perlinNoise(vec2 uv) {
    float surfletSum = 0.f;

    uv *= 10;
    // Iterate over four integer corners surrounding uv
    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = 0; dy <= 1; ++dy) {
            surfletSum += surflet(uv, floor(uv) + vec2(dx, dy));
        }
    }

    return surfletSum;
}

float worleyNoise(vec2 uv) {
    uv *= 7.5;
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);
    float minDist = 5;
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vec2 neighbor = vec2(float(x), float(y));
            vec2 point = random2(uvInt + neighbor);
            vec2 diff = neighbor + point - uvFract;
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }

    return minDist;
}

void main()
{
    float time = u_Time * 0.01;
    while (time > 100)
    {
        time -= 100;
    }
    vec2 uv = fs_UV;
    color = texture(u_sampler, fs_UV);

    if (u_postType == 1)
    {

        uv.x += sin(uv.y * 15. + time * 2.) / u_Dimensions[0];
        uv.y += cos(uv.x * 10. + time * 2.) / u_Dimensions[1];

        uv.x += sin((uv.y+uv.x) * 15. + time * 2.) / (180. + (2. * sin(time)));
        uv.y += cos((uv.y+uv.x) * 15. + time * 2.) / (200. + (2. * sin(time)));
        color = vec4((texture(u_sampler, uv).rgb + vec3(0.05f, 0.05f, 0.75f)), 1);
    }
    else if(u_postType == 2)
    {
        float minDistX1 = perlinNoise(uv - vec2(coeff/u_Dimensions[0], 0));
        float minDistX2 = perlinNoise(uv + vec2(coeff/u_Dimensions[0], 0));
        float minDistY1 = perlinNoise(uv - vec2(0, coeff/u_Dimensions[1]));
        float minDistY2 = perlinNoise(uv + vec2(0, coeff/u_Dimensions[1]));

        vec2 grad = vec2(minDistX2 - minDistX1, minDistY2 - minDistY1);
        vec4 textureColor = texture(u_sampler, grad + uv);

        color = mix(textureColor, vec4(1.f, 0.f, 0.f, textureColor.a), 0.5);
    }
    else if(u_postType == 3)
    {
        float minDistX1 = worleyNoise(uv - vec2(coeff/u_Dimensions[0], 0));
        float minDistX2 = worleyNoise(uv + vec2(coeff/u_Dimensions[0], 0));
        float minDistY1 = worleyNoise(uv - vec2(0, coeff/u_Dimensions[1]));
        float minDistY2 = worleyNoise(uv + vec2(0, coeff/u_Dimensions[1]));

        vec2 grad = vec2(minDistX2 - minDistX1, minDistY2 - minDistY1);
        vec4 textureColor = texture(u_sampler, grad + uv);

        color = mix(textureColor, vec4(0.1f, 0.1f, 0.75f, 0), 0.2);
    }
    else if(u_postType == 4)
    {
        vec3 white = vec3(0.9, 0.95, 0.95);
        vec2 center = vec2(0.5, 0.5);
        float distance = sqrt(pow((fs_UV - center).x, 2) + pow((fs_UV - center).y, 2));
        color = mix(color, vec4(white.rgb, color.a), distance);
    }

}
