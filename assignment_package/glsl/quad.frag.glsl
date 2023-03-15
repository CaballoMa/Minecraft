#version 330 core
in vec2 TexCoords;

uniform sampler2D u_sampler;

out vec4 FragColor;

void main()
{

    FragColor = texture(u_sampler, TexCoords);
    //FragColor = vec4(TexCoords, 0, 1);
}
