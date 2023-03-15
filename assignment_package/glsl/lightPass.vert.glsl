#version 330

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;
uniform mat4 u_lightViewProj;
in vec4 vs_Pos;
in vec4 vs_Nor;
in vec4 vs_UV;

out vec4 fs_Pos;
out vec4 fs_Nor;
out vec4 fs_UV;
out vec4 fs_LightSpacePos;

void main()
{
    fs_Pos = u_Model * vs_Pos;
    fs_UV = vs_UV;

    fs_Nor = u_ModelInvTr * vs_Nor;
    vec4 modelposition = u_Model * vs_Pos;
    fs_LightSpacePos = u_lightViewProj *  u_Model * vs_Pos;
    gl_Position = u_ViewProj * modelposition;

}
