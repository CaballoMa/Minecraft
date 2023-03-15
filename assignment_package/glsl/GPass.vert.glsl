#version 330

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;
in vec4 vs_Pos;
in vec4 vs_Nor;
in vec4 vs_UV;

out vec4 fs_Pos;
out vec4 fs_Nor;
out vec4 fs_UV;

void main()
{
    fs_Pos = u_Model * vs_Pos;
    fs_UV = vs_UV;
    fs_Nor = u_ModelInvTr * vs_Nor;
    gl_Position = u_ViewProj * u_Model * vs_Pos;

}
