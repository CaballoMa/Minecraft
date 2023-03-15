#version 330
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.

uniform vec4 u_Color; // The color with which to render this instance of geometry.

uniform sampler2D u_sampler;
uniform int u_Time;
uniform vec3 u_PlayerPos;

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_WorldPos;
//in vec4 fs_Col;
in vec4 fs_UV;

layout(location = 0) out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

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

void main()
{
    // Material base color (before shading)

    vec2 uv = fs_UV.xy;
    vec3 sunColor = vec3(0.5,0.5,0.55);

    float specularTerm = 0;
    if(fs_UV.z != 0){
        uv.x += mod(u_Time,  200) * 0.0625 * 0.0;
        float shininess = 32;
        vec3 view_dir = u_PlayerPos - vec3(fs_Pos);
        vec3 H = normalize((normalize(view_dir) + normalize(fs_LightVec.xyz)));
        specularTerm = pow(dot(normalize(vec3(fs_Nor)), H), shininess);
        specularTerm = clamp(specularTerm, 0.f, 1.f);
    }


    specularTerm *= 0.4;
    vec4 diffuseColor = texture(u_sampler, uv);
    float alpha = diffuseColor.a;
    // Calculate the diffuse term for Lambert shading
    float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
    // Avoid negative lighting values
    diffuseTerm = clamp(diffuseTerm, 0, 1);

    float ambientTerm = 0.2;

    float lightIntensity = diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                        //to simulate ambient lighting. This ensures that faces that are not
                                                        //lit by our point light are not completely black.

    //fog
    float dist = length(fs_Pos.xyz - u_PlayerPos.xyz) * 0.01f;
    vec4 color = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
    color = mix(color, vec4(sunColor, 1.f), pow(smoothstep(0, 1, min(1, dist)), 2));
    out_Col = vec4(color.rgb + vec3(specularTerm), alpha + specularTerm);
}
