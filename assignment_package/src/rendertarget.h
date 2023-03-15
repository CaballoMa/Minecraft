#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "openglcontext.h"

class Rendertarget
{
public:
    Rendertarget(OpenGLContext* Context);
    ~Rendertarget(){}
    void create(float width, float height);
    void bind();
    GLuint m_renderedTexture;
private:
    GLuint m_frameBuffer;

    GLuint m_depthRenderBuffer;
    OpenGLContext* context;
};

#endif // RENDERTARGET_H
