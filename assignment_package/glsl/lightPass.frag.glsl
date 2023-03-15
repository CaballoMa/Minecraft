#version 330 core
out vec4 FragColor;

uniform sampler2D u_sampler;
uniform sampler2D u_shadowMap;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform int u_Time;

in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_UV;
in vec4 fs_LightSpacePos;

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

float mySmoothStep(float a, float b, float t) {
    t = smoothstep(0, 1, t);
    return mix(a, b, t);
}

float cubicTriMix(vec3 p) {
    vec3 pFract = fract(p);
    float llb = random1(floor(p) + vec3(0,0,0));
    float lrb = random1(floor(p) + vec3(1,0,0));
    float ulb = random1(floor(p) + vec3(0,1,0));
    float urb = random1(floor(p) + vec3(1,1,0));

    float llf = random1(floor(p) + vec3(0,0,1));
    float lrf = random1(floor(p) + vec3(1,0,1));
    float ulf = random1(floor(p) + vec3(0,1,1));
    float urf = random1(floor(p) + vec3(1,1,1));

    float mixLoBack = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront = mySmoothStep(ulf, urf, pFract.x);

    float mixLo = mySmoothStep(mixLoBack, mixLoFront, pFract.z);
    float mixHi = mySmoothStep(mixHiBack, mixHiFront, pFract.z);

    return mySmoothStep(mixLo, mixHi, pFract.y);
}

float fbm(vec3 p) {
    float amp = 0.5;
    float freq = 4.0;
    float sum = 0.0;
    for(int i = 0; i < 8; i++) {
        sum += cubicTriMix(p * freq) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }
    return sum;
}



float ShadowCalculation(vec4 fragPosLightSpace)
{
    float value = abs(dot(normalize(lightPos), vec3(1,0,0)));
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    //projCoords.y = 1 - projCoords.y;
    float currentDepth = projCoords.z;
    float closestDepth = texture(u_shadowMap, projCoords.xy).r;
    float shadow = currentDepth - 0.005 > closestDepth  ? 1.0 : 0.0;
    //return 0;
    return shadow;
}

void main(void)
{
//    vec3 color = texture(u_sampler, fs_UV.xy).rgb;
//    vec3 normal = normalize(fs_Nor.xyz);
//    vec3 lightColor = vec3(1.0);
//    // ambient
//    vec3 ambient = 0.15 * lightColor;
//    // diffuse
//    vec3 lightDir = normalize(lightPos - fs_Pos.xyz);
//    float diff = max(dot(lightDir, normal), 0.0);
//    vec3 diffuse = diff * lightColor;
//    // specular
//    vec3 viewDir = normalize(viewPos - fs_Pos.xyz);
//    vec3 halfwayDir = normalize(lightDir + viewDir);
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
//    vec3 specular = spec * lightColor;
//    // calculate shadow
//    float shadow = ShadowCalculation(fs_LightSpacePos);
//    vec3 lighting = (ambient + (1-shadow) * (diffuse + specular)) * color;

//    FragColor = vec4(lighting, 1.0);
    //FragColor = vec4(color, 1);

    vec2 uv = fs_UV.xy;
    //every 200 add 1/16 to uv.x
    if(fs_UV.z != 0){
        uv.x += u_Time % 200 * 0.0625 * 0.005;
    }

    vec4 diffuseColor = texture(u_sampler, uv);
    float alpha = diffuseColor.a;

    // Calculate the diffuse term for Lambert shading
    vec4 lightDir = normalize(vec4(0.5, 1, 0.75, 0));//normalize(vec4(lightPos, 1) - fs_Pos);
    float diffuseTerm = dot(normalize(fs_Nor), normalize(lightDir));
    // Avoid negative lighting values
    diffuseTerm = clamp(diffuseTerm, 0, 1);

    float ambientTerm = 0.2;
    float shadow = ShadowCalculation(fs_LightSpacePos);
    float lightIntensity = diffuseTerm + ambientTerm;
    // Compute final shaded color
    FragColor = (1-shadow) * vec4(diffuseColor.rgb * lightIntensity, alpha);
}
