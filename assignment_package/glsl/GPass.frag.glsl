#version 330 core
uniform sampler2D u_sampler;
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_UV;

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;
layout (location = 3) out vec4 gDepth;

void main(void)
{
    vec4 albedo = texture(u_sampler, fs_UV.xy);
    vec4 normal = normalize(fs_Nor);
    gAlbedo = albedo;
    gNormal = normal;
    gPosition = fs_Pos;
    gDepth = vec4(gl_FragCoord.z, 0, 0, 1);
}
