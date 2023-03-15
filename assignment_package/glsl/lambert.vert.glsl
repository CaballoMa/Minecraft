#version 330
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself

uniform vec4 u_Color;       // When drawing the cube instance, we'll set our uniform color to represent different block types.
uniform int u_Time;
uniform vec4 u_lightDir ;

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.
in vec4 vs_UV;

out vec4 fs_WorldPos;
out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
//out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_UV;


const float PI = 3.14159;


int rand() // RAND_MAX assumed to be 32767
{
    int seed = u_Time * 1103515245 + 12345;
    return (seed/65536) % 32768;
}

void main()
{
    fs_Pos = u_Model * vs_Pos;
    //fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation
    fs_UV = vs_UV;

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.

    vec4 modelposition = u_Model * vs_Pos;   // Temporarily store the transformed vertex positions for use below

    vec4 norm = vec4(invTranspose * vec3(vs_Nor), 0);

    if(fs_UV.z != 0)
    {
        modelposition.y -= 0.5f;
        float wave1 = sin((modelposition.x + modelposition.z)/2 + (u_Time * 0.015) * PI) / 4;
        float wave2 = sin(modelposition.x + (u_Time * 0.02) * PI) / 4;
        float wave3 = sin(modelposition.z + (u_Time * 0.02) * PI) / 4;

        modelposition.y += 0.4 * wave2 + 1.2 * wave1 + 0.4 * wave3;

        vec3 xMove = vec3(1, cos(0.3f * wave2 + 0.8f * wave1 + 0.3f * wave3) / 2., 0);
        vec3 zMove = vec3(0,0,1);
        norm = vec4(-normalize(cross(xMove, zMove)), 0);
    }

    fs_Pos = modelposition;
    fs_Nor = norm;

    fs_LightVec = u_lightDir;  // Compute the direction in which the light source lies

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
    fs_WorldPos = u_ViewProj * fs_Pos;
}
