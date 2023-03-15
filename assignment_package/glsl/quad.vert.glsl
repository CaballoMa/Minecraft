#version 330
in vec4 vs_Pos;
in vec2 vs_UV;

out vec2 TexCoords;

void main()
{
    gl_Position = vs_Pos;
    TexCoords = vs_UV;
}
