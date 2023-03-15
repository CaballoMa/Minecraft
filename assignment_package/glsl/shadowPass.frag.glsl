#version 330
layout(location = 0) out vec4 depth;
////layout(location = 0) out float fragmentdepth;
//uniform ivec2 u_Dimensions;
//uniform mat4 invPersMatrix;

in vec4 fs_Pos;

void main()
{
//    vec4 ndcPos;
//    ndcPos.xy = ((2.0 * gl_FragCoord.xy) - (2.0 * vec2(0,0))) / u_Dimensions - 1;
//    ndcPos.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) /
//        (gl_DepthRange.far - gl_DepthRange.near);
//    ndcPos.w = 1.0;

//    vec4 clipPos = ndcPos / gl_FragCoord.w;
//    vec4 eyePos = invPersMatrix * clipPos;

//    depth = vec4(eyePos.zzz, 1);
    float dep = gl_FragCoord.z;//fs_Pos.z / fs_Pos.w;
    depth = vec4(dep, dep,dep,1);

    //fragmentdepth = gl_FragCoord.z ;
}
