#include "rendertarget.h"

Rendertarget::Rendertarget(OpenGLContext* Context) : context(Context)
{

}

void Rendertarget::create(float width, float height)
{
    // Initialize the frame buffers and render textures
    context->glGenFramebuffers(1, &m_frameBuffer);
    context->glGenTextures(1, &m_renderedTexture);
    context->glGenRenderbuffers(1, &m_depthRenderBuffer);

    context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    // Bind our texture so that all functions that deal with textures will interact with this one
    context->glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
    // Give an empty image to OpenGL ( the last "0" )
    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)0);
    //context->glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, width, height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    // Set the render settings for the texture we've just created.
    // Essentially zero filtering on the "texture" so it appears exactly as rendered
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Clamp the colors at the edge of our texture
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Initialize our depth buffer
    context->glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
    context->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height );
    context->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);

    // Set m_renderedTexture as the color output of our frame buffer
    context->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);

    // Sets the color output of the fragment shader to be stored in GL_COLOR_ATTACHMENT0, which we previously set to m_renderedTextures[i]
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    context->glDrawBuffers(1, drawBuffers); // "1" is the size of drawBuffers

    if(context->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qDebug() << "Frame buffer did not initialize correctly..." ;
        context->printGLErrorLog();
    }
}

void Rendertarget::bind()
{
    context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
}
