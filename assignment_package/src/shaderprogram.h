#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <openglcontext.h>
#include <glm_includes.h>
#include <glm/glm.hpp>

#include "drawable.h"


class ShaderProgram
{
public:
    GLuint vertShader; // A handle for the vertex shader stored in this shader program
    GLuint fragShader; // A handle for the fragment shader stored in this shader program
    GLuint prog;       // A handle for the linked shader program stored in this class

    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrNor; // A handle for the "in" vec4 representing vertex normal in the vertex shader
    int attrCol; // A handle for the "in" vec4 representing vertex color in the vertex shader
    int attrPosOffset; // A handle for a vec3 used only in the instanced rendering shader
    int attrUV;

    int unifModel; // A handle for the "uniform" mat4 representing model matrix in the vertex shader
    int unifModelInvTr; // A handle for the "uniform" mat4 representing inverse transpose of the model matrix in the vertex shader
    int unifViewProj; // A handle for the "uniform" mat4 representing combined projection and view matrices in the vertex shader
    int unifColor; // A handle for the "uniform" vec4 representing color of geometry in the vertex shader
    int unifLightViewProj; // A handle for camera space view proj
    int unifSampler2D;
    int unifTime;
    int unifShadowSampler;
    int unifCamPos;
    int unifLightPos;

    int unifAlbedoSampler;
    int unifNormalSampler;
    int unifNoiseSampler;

    int unifPostType;
    int unifPlayerPos;
    int unifDimensions;
    int unifLightDir;

public:
    ShaderProgram(OpenGLContext* context);
    // Sets up the requisite GL data and shaders from the given .glsl files
    void create(const char *vertfile, const char *fragfile);
    // Tells our OpenGL context to use this shader to draw things
    void useMe();
    // Pass the given model matrix to this shader on the GPU
    void setModelMatrix(const glm::mat4 &model);
    // Pass the given Projection * View matrix to this shader on the GPU
    void setViewProjMatrix(const glm::mat4 &vp);
    // Pass time to GPU
    void setTime(int t);
    // Pass texture handler
    void setTerrainSampler(int TextureSlot);
    // Pass the given color to this shader on the GPU
    void setGeometryColor(glm::vec4 color);

    // Pass camera space viewproj
    void setLightViewProjMatrix(const glm::mat4 &vp);
    void setPlayerPos(glm::vec3 pos);

    // From learnopengl
    void setInt(const std::string &name, int value) const;

    // Draw the given object to our screen using this ShaderProgram's shaders
    void draw(Drawable &d);
    // Draw the given object to our screen multiple times using instanced rendering
    void drawInstanced(InstancedDrawable &d);
    void drawInterleave(Drawable &d);

    // opaque and translucent
    void drawOpaque(Drawable &d);
    void drawTranslucent(Drawable &d);

    void drawSky(Drawable &d);
    // Utility function used in create()
    char* textFileRead(const char*);
    // Utility function that prints any shader compilation errors to the console
    void printShaderInfoLog(int shader);
    // Utility function that prints any shader linking errors to the console
    void printLinkInfoLog(int prog);

    void drawArray(Drawable &d, int slot);
    void setShadowSampler(int slot);
    void setLightPos(glm::vec3 pos);
    void setLightDir(glm::vec4 dir);
    void setCamPos(glm::vec3 pos);
    void setNoiseSampler(int TextureSlot);
    void setPostType(int type);
    void setDimensions(glm::ivec2 dims);

    QString qTextFileRead(const char*);

private:
    OpenGLContext* context;   // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                            // we need to pass our OpenGL context to the Drawable in order to call GL functions
                            // from within this class.
};


#endif // SHADERPROGRAM_H
