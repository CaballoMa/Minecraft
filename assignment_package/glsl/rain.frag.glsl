#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments
uniform int u_Time;
in vec4 fs_Col;

out vec4 out_Col;

void main()
{
    out_Col = vec4(vec3(fs_Col),0.6f);
}
