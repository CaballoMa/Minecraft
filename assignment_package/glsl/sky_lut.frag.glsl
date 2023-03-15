#version 330
const float M_PI = 3.1415926f;

uniform mat4 InverseViewMatrix;
uniform mat4 InverseProjectionMatrix;
uniform mat4 InverseViewProj;
uniform vec3 IncomingLight;
uniform vec3 WorldSpaceCameraPos;
uniform vec3 lightDir;
uniform vec3 ScatteringR;
uniform vec3 ScatteringM;
uniform vec3 ExtinctionR;
uniform vec3 ExtinctionM;
uniform vec2 DensityScaleHeight;
uniform ivec2 u_Dimensions;
uniform float PlanetRadius;
uniform float AtmosphereHeight;
uniform float SunIntensity;
uniform float MieG;
uniform float DistanceScale;
uniform float Angle;
uniform int   u_Time;

uniform sampler2D u_sampler;
uniform sampler2D u_albedoSampler;
uniform sampler2D u_NoiseSampler;

in vec2 fs_UV;
out vec4 finalColor;


vec2 RaySphereIntersection(vec3 rayOrigin, vec3 rayDir, vec3 sphereCenter, float sphereRadius)
{
    rayOrigin -= sphereCenter;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(rayOrigin, rayDir);
    float c = dot(rayOrigin, rayOrigin) - (sphereRadius * sphereRadius);
    float d = b * b - 4 * a * c;
    if (d < 0)
    {
        return vec2(-1, -1);
    }
    else
    {
        d = sqrt(d);
        return vec2(-b - d, -b + d) / (2 * a);
    }
}

vec3 GetWorldSpacePosition(vec2 uv, float depth)
{
    vec2 ndc = uv * 2.0 - 1.0;
    //vec4 p = vec4(ndc.xy, depth * 2.0 - 1.0, 1);
    vec4 p = vec4(ndc.xy, 1, 1);
    p *= 1000.0;
//    vec4 positionViewSpace = InverseViewMatrix * vec4(ndc, depth, 1.0);
//    positionViewSpace /= positionViewSpace.w;

//    vec4 positionWorldSpace = InverseProjectionMatrix * vec4(positionViewSpace.xyz, 1.0);
//    return positionWorldSpace.xyz;
    vec4 positionWorldSpace = InverseViewProj * p;
    return positionWorldSpace.xyz;
}

//----- Input
// position		view vector sample position
// lightDir		sun light direction

//----- Output :
// opticalDepthCP:	dcp
vec2 lightSampleing(vec3 position, vec3 lightDir)
{
    vec2 opticalDepthCP;

    vec3 rayStart = position;
    vec3 rayDir = lightDir;

    vec3 planetCenter = vec3(0, -PlanetRadius, 0);
    vec2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, PlanetRadius + AtmosphereHeight);
    vec3 rayEnd = rayStart + rayDir * intersection.y;

    // compute density along the ray
    float stepCount = 50;// 250;
    vec3 step = (rayEnd - rayStart) / stepCount;
    float stepSize = length(step);
    vec2 density = vec2(0, 0);

    for (float s = 0.5; s < stepCount; s += 1.0)
    {
        vec3 position = rayStart + step * s;
        float height = abs(length(position - planetCenter) - PlanetRadius);
        vec2 localDensity = exp(-(vec2(height, height) / DensityScaleHeight));

        density += localDensity * stepSize;
    }

    opticalDepthCP = density;
    return opticalDepthCP;
}

//----- Input
// position     sample position
// lightDir	light direction

//----- Output :
//dpa
//dcp
bool GetAtmosphereDensityRealtime(vec3 position, vec3 planetCenter, vec3 lightDir, inout vec2 dpa, inout vec2 dpc)
{
    float height = length(position - planetCenter) - PlanetRadius;
    dpa = exp(-vec2(height, height) / DensityScaleHeight);

    dpc = lightSampleing(position, lightDir);
    return true;
}

void ComputeLocalInscattering(vec2 localDensity, vec2 densityPA, vec2 densityCP, inout vec3 localInscatterR, inout vec3 localInscatterM)
{
    vec2 densityCPA = densityCP + densityPA;

    vec3 Tr = densityCPA.x * ExtinctionR;
    vec3 Tm = densityCPA.y * ExtinctionM;

    vec3 extinction = exp(-(Tr + Tm));

    localInscatterR = localDensity.x * extinction;
    localInscatterM = localDensity.y * extinction;
}

void ApplyPhaseFunction(inout vec3 scatterR, inout vec3 scatterM, float cosAngle)
{
    // rayleigh
    float phase = (3.0 / (16.0 * M_PI)) * (1 + (cosAngle * cosAngle));
    scatterR *= phase;

    // mie
    float g = MieG;
    float g2 = g * g;
    phase = (1.0 / (4.0 * M_PI)) * ((3.0 * (1.0 - g2)) / (2.0 * (2.0 + g2))) * ((1 + cosAngle * cosAngle) / (pow((1 + g2 - 2 * g * cosAngle), 3.0 / 2.0)));
    scatterM *= phase;
}


//----- Input
// rayStart		starting point
// rayDir		ray direction
// rayLength		Length of AB
// planetCenter		earth center position
// distanceScale	world coordinate scale
// lightdir		sun light direction
// sampleCount		sample count

//----- Output :
// extinction           T(PA)
// inscattering:	Inscatering
vec4 IntegrateInscatteringRealtime(
        vec3 rayStart,
        vec3 rayDir,
        float rayLength,
        vec3 planetCenter,
        float distanceScale,
        vec3 lightDir,
        float sampleCount,
        out vec4 extinction)
{
    vec3 step = rayDir * (rayLength / sampleCount);
    float stepSize = length(step) * distanceScale;

    vec2 densityCP = vec2(0, 0);
    vec3 scatterR = vec3(0, 0, 0);
    vec3 scatterM = vec3(0, 0, 0);

    vec2 localDensity;
    vec2 densityPA;

    vec2 prevLocalDensity;
    vec3 prevLocalInscatterR, prevLocalInscatterM;
    GetAtmosphereDensityRealtime(rayStart, planetCenter, lightDir, prevLocalDensity, densityCP);

    ComputeLocalInscattering(prevLocalDensity, densityCP, densityPA, prevLocalInscatterR, prevLocalInscatterM);

    // P - current integration point
    // A - camera position
    // C - top of the atmosphere
    for (float s = 1.0; s < sampleCount; s += 1)
    {
        vec3 p = rayStart + step * s;

        GetAtmosphereDensityRealtime(p, planetCenter, lightDir, localDensity, densityCP);
        densityPA += (localDensity + prevLocalDensity) * (stepSize / 2.0);
        vec3 localInscatterR, localInscatterM;
        ComputeLocalInscattering(localDensity, densityCP, densityPA, localInscatterR, localInscatterM);

        scatterR += (localInscatterR + prevLocalInscatterR) * (stepSize / 2.0);
        scatterM += (localInscatterM + prevLocalInscatterM) * (stepSize / 2.0);

        prevLocalInscatterR = localInscatterR;
        prevLocalInscatterM = localInscatterM;

        prevLocalDensity = localDensity;
    }
    vec3 m = scatterM;
    // phase function
    ApplyPhaseFunction(scatterR, scatterM, dot(normalize(rayDir), normalize(lightDir.xyz)));
    //scatterR = 0;
    vec3 lightInscatter = (scatterR * ScatteringR + scatterM * ScatteringM) * IncomingLight.xyz;
    //lightInscatter += RenderSun(m, dot(rayDir, -lightDir.xyz)) * _SunIntensity;
    vec3 lightExtinction = exp(-(densityCP.x * ExtinctionR + densityCP.y * ExtinctionM));

    extinction = vec4(lightExtinction, 1.0);
    //return vec4(m, 1);
    return vec4(lightInscatter, 1);

}

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

/**** Cloud Ray Marching****/
float cloudRayMarching(vec3 startPoint, vec3 dir)
{
    vec3 testPoint = startPoint;
    float sum = 0.0f;

    for(int i = 0; i < 256; i++){
        testPoint += dir;
        if(testPoint.x > -10 && testPoint.x < 10
        && testPoint.y > -10 && testPoint.y < 10
        && testPoint.z > -10 && testPoint.z < 10 ){
            sum += 0.01;
        }
    }
    return sum;
}

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec3 random3( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1, 311.7, 191.999)),
                          dot(p,vec3(269.5, 183.3, 765.54)),
                          dot(p, vec3(420.69, 631.2,109.21))))
                 *43758.5453);
}

float WorleyNoise3D(vec3 p)
{
    // Tile the space
    vec3 pointInt = floor(p);
    vec3 pointFract = fract(p);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3(pointInt + neighbor);

                // Animate the point
                point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(diff);
                minDist = min(minDist, dist);
            }
        }
    }
    return minDist;
}

float WorleyNoise(vec2 uv)
{
    // Tile the space
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));

            // Random point inside current neighboring cell
            vec2 point = random2(uvInt + neighbor);

            // Animate the point
            point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

            // Compute the distance b/t the point and the fragment
            // Store the min dist thus far
            vec2 diff = neighbor + point - uvFract;
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

float worleyFBM(vec3 uv) {
    float sum = 0;
    float freq = 4;
    float amp = 0.5;
    for(int i = 0; i < 8; i++) {
        sum += WorleyNoise3D(uv * freq) * amp;
        freq *= 2;
        amp *= 0.5;
    }
    return sum;
}

float bottom =  13;
float top = 20;
float width = 5;

float getDensity(vec3 pos) {
    vec2 coord = pos.xz * 0.025;
    float noise = texture(u_NoiseSampler, coord).x;
    return noise;
}

vec4 getCloud(vec3 worldPos, vec3 cameraPos) {
    vec3 direction = normalize(worldPos - cameraPos);
    vec3 step = direction * 0.025;
    vec4 colorSum = vec4(0);
    vec3 point = cameraPos;

    // ray marching
    for(int i=0; i<100; i++) {
        point += step;

        if(point.y<bottom) {
            point += direction * (abs(bottom - cameraPos.y) / abs(direction.y));
        }

        if(top<point.y) {
            point += direction * (abs(cameraPos.y - top) / abs(direction.y));
        }

        // if something block the cloud, return
        float len1 = length(point - cameraPos);
        float len2 = length(worldPos - cameraPos);
        if(len2<len1) {
            return vec4(0);
        }

        if(bottom>point.y || point.y>top || -width>point.x || point.x>width || -width>point.z || point.z>width) {
            continue;
        }

        float density = getDensity(point) * 0.1;
//        if(density < 0.2)
//            density = 0.0;
        vec4 color = vec4(0.9, 0.8, 0.7, 1.0) * density;
        colorSum = colorSum + color * (1.0 - colorSum.a);
    }

    return colorSum;
}

void main(void)
{


    vec2 uv = gl_FragCoord.xy / u_Dimensions;
    float deviceZ = texture(u_sampler, fs_UV).x;
    vec3 positionWorldSpace  = GetWorldSpacePosition(uv, deviceZ);

    vec3 rayStart = WorldSpaceCameraPos;
    vec3 rayDir = positionWorldSpace - WorldSpaceCameraPos;
    float rayLength = length(rayDir);
    rayDir = rayDir / rayLength;


//    float sunRadius = 10000;
//    vec3 sunPosition = vec3(sunRadius * cos(Angle), sunRadius * sin(Angle), 0 );
//    vec3 lightDir = sunPosition - WorldSpaceCameraPos;


//    if (deviceZ > 0.9999)
//    {
    rayLength = 1e20;
    vec3 planetCenter = vec3(0, -PlanetRadius, 0);
    vec2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, PlanetRadius + AtmosphereHeight);
    rayLength = min(intersection.y, rayLength);

    intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, PlanetRadius);
    if(intersection.x > 0)
        rayLength = min(rayLength, intersection.x);

    vec4 extinction;
    //SunIntensity = 0;

    vec4 color = IntegrateInscatteringRealtime(rayStart,
                                               rayDir,
                                               rayLength,
                                               planetCenter,
                                               1,
                                               lightDir,
                                               16,
                                               extinction);



    //gamma correction and tonemap
    vec3 sRGB = ACESFilm(color.xyz);
    finalColor = vec4(pow(sRGB.rgb, vec3(1/2.2)), 1);


//    vec4 cloud = getCloud(positionWorldSpace, WorldSpaceCameraPos);
//    finalColor.rgb = finalColor.rgb*(1.0 - cloud.a) + cloud.rgb;











//    }else{
//        finalColor = texture(u_albedoSampler, fs_UV);
//    }



//    if (deviceZ > 0.9999)
//    {
//        rayLength = 1e20;
//    }


//    vec3 planetCenter = vec3(0, -PlanetRadius, 0);
//    vec2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, PlanetRadius + AtmosphereHeight);

//    finalColor = vec4(intersection + vec2(1,1), 0, 1);
//    return;
//    rayLength = min(intersection.y, rayLength);

//    vec4 extinction;
//    //SunIntensity = 0;

//    if (deviceZ > 0.9999)
//    {
//        finalColor = IntegrateInscatteringRealtime(rayStart, rayDir, rayLength, planetCenter, 1, lightDir, 16, extinction);
//    }

    //finalColor = vec4(positionWorldSpace, 1);
    //float depth = gl_FragCoord.z;
    //finalColor = vec4(depth, depth, depth,1);
}
