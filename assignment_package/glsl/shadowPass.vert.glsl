#version 330
uniform mat4 u_Model;
uniform mat4 u_lightViewProj;
uniform mat4 u_ViewProj;

in vec4 vs_Pos;
out vec4 fs_Pos;

void main()
{
    gl_Position = u_lightViewProj * u_Model * vs_Pos;
    fs_Pos = u_lightViewProj * u_Model * vs_Pos;
//    gl_Position = u_ViewProj * u_Model * vs_Pos;
//    fs_Pos = u_ViewProj * u_Model * vs_Pos;
}
