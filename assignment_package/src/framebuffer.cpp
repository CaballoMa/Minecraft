#include "framebuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(OpenGLContext *context,
                         unsigned int width, unsigned int height, unsigned int devicePixelRatio)
    : mp_context(context), m_frameBuffer(-1),
      m_outputTexture(-1), m_depthRenderBuffer(-1),
      m_width(width), m_height(height), m_devicePixelRatio(devicePixelRatio), m_created(false)
{}

void FrameBuffer::resize(unsigned int width, unsigned int height, unsigned int devicePixelRatio) {
    m_width = width;
    m_height = height;
    m_devicePixelRatio = devicePixelRatio;
}

void FrameBuffer::create() {
    // Initialize the frame buffers and render textures
    mp_context->glGenFramebuffers(1, &m_frameBuffer);
    mp_context->glGenTextures(1, &m_outputTexture);
    mp_context->glGenRenderbuffers(1, &m_depthRenderBuffer);

    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    // Bind our texture so that all functions that deal with textures will interact with this one
    mp_context->glBindTexture(GL_TEXTURE_2D, m_outputTexture);
    // Give an empty image to OpenGL ( the last "0" )
    mp_context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width * m_devicePixelRatio, m_height * m_devicePixelRatio, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // Set the render settings for the texture we've just created.
    // Essentially zero filtering on the "texture" so it appears exac.tly as rendered
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Clamp the colors at the edge of our texture
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Initialize our depth buffer
    mp_context->glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
    mp_context->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    mp_context->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);

    // Set m_renderedTexture as the color output of our frame buffer
    mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_outputTexture, 0);

    // Sets the color output of the fragment shader to be stored in GL_COLOR_ATTACHMENT0,
    // which we previously set to m_renderedTexture
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    mp_context->glDrawBuffers(1, drawBuffers); // "1" is the size of drawBuffers

    m_created = true;
    if(mp_context->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        m_created = false;
        std::cout << "Frame buffer did not initialize correctly..." << std::endl;
        mp_context->printGLErrorLog();
    }
    //learnopengl called this function
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::destroy() {
    if(m_created) {
        m_created = false;
        mp_context->glDeleteFramebuffers(1, &m_frameBuffer);
        mp_context->glDeleteTextures(1, &m_outputTexture);
        mp_context->glDeleteRenderbuffers(1, &m_depthRenderBuffer);

        mp_context->glDeleteTextures(1, &albedoTexture);
        mp_context->glDeleteTextures(1, &normalTexture);
        mp_context->glDeleteTextures(1, &positionTexture);
    }
}

void FrameBuffer::bindFrameBuffer() {
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    //mp_context->glViewport(0,0,1024,1024);
    mp_context->glViewport(0,0,m_width*m_devicePixelRatio, m_height*m_devicePixelRatio);
}

void FrameBuffer::bindToTextureSlot(unsigned int slot) {
    m_textureSlot = slot;
    mp_context->glActiveTexture(GL_TEXTURE0 + slot);
    mp_context->glBindTexture(GL_TEXTURE_2D, m_outputTexture);
}

unsigned int FrameBuffer::getTextureSlot() const {
    return m_textureSlot;
}

void FrameBuffer::createDepthFB()
{
//    // Initialize the frame buffers and render textures
//    mp_context->glGenFramebuffers(1, &m_frameBuffer);

//    mp_context->glGenTextures(1, &m_outputTexture);
//    mp_context->glBindTexture(GL_TEXTURE_2D, m_outputTexture);
//    mp_context->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
////    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
////    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


//    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
//    mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_outputTexture, 0);

//    //mp_context->glDrawBuffer(GL_NONE);
//    //mp_context->glDrawBuffers(0, NULL);
////    glDrawBuffer(GL_NONE);
////    glReadBuffer(GL_NONE);
//    GLenum drawBuffers[1] = {GL_NONE};
//    mp_context->glDrawBuffers(1, drawBuffers); // "1" is the size of drawBuffers
//    mp_context->glReadBuffer(GL_NONE);

//    //mp_context->glBindFramebuffer(GL_FRAMEBUFFER, 0);

//    m_created = true;
//    if(mp_context->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//    {
//        m_created = false;
//        std::cout << "Frame buffer did not initialize correctly..." << std::endl;
//        mp_context->printGLErrorLog();
//    }

//    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, 0);


    float w = m_width * m_devicePixelRatio;
    float h = m_height * m_devicePixelRatio;

    mp_context->glGenFramebuffers(1, &m_frameBuffer);
    mp_context->glGenTextures(1, &m_outputTexture);
    mp_context->glGenTextures(1, &m_depthTexture);

    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    mp_context->glBindTexture(GL_TEXTURE_2D, m_outputTexture);
    mp_context->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);

    mp_context->glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    mp_context->glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, w, h);

    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Set m_renderedTexture as the color output of our frame buffer
    mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_outputTexture, 0);
    mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);

    // Sets the color output of the fragment shader to be stored in GL_COLOR_ATTACHMENT0,
    // which we previously set to m_renderedTexture
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    mp_context->glDrawBuffers(1, drawBuffers); // "1" is the size of drawBuffers

    m_created = true;
    if(mp_context->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        m_created = false;
        std::cout << "Frame buffer did not initialize correctly..." << std::endl;
        mp_context->printGLErrorLog();
    }
}


void FrameBuffer::createGBuffer()
{
    float w = m_width * m_devicePixelRatio;
    float h = m_height * m_devicePixelRatio;
    // Initialize the frame buffers and render textures
    mp_context->glGenFramebuffers(1, &m_frameBuffer);
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

    //albedo texture
    mp_context->glGenTextures(1, &albedoTexture);
    mp_context->glBindTexture(GL_TEXTURE_2D, albedoTexture);
    mp_context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    mp_context->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, albedoTexture, 0);
    //normal texture
    mp_context->glGenTextures(1, &normalTexture);
    mp_context->glBindTexture(GL_TEXTURE_2D, normalTexture);
    mp_context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    mp_context->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
    //position texture
    mp_context->glGenTextures(1, &positionTexture);
    mp_context->glBindTexture(GL_TEXTURE_2D, positionTexture);
    mp_context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    mp_context->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, positionTexture, 0);
    //depth texture
    mp_context->glGenTextures(1, &depthTexture);
    mp_context->glBindTexture(GL_TEXTURE_2D, depthTexture);
    mp_context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    mp_context->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, depthTexture, 0);
//    mp_context->glGenTextures(1, &depthTexture);
//    mp_context->glBindTexture(GL_TEXTURE_2D, depthTexture);
//    mp_context->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//    mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, depthTexture, 0);

//    mp_context->glGenRenderbuffers(1, &m_depthRenderBuffer);
//    mp_context->glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
//    mp_context->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
//    mp_context->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);


    // Sets the color output of the fragment shader
    GLenum drawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    mp_context->glDrawBuffers(4, drawBuffers);

    m_created = true;
    if(mp_context->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        m_created = false;
        std::cout << "Frame buffer did not initialize correctly..." << std::endl;
        mp_context->printGLErrorLog();
    }
    //learnopengl called this function
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void FrameBuffer::bindAlbedo(unsigned int slot)
{
    mp_context->glActiveTexture(GL_TEXTURE0 + slot);
    mp_context->glBindTexture(GL_TEXTURE_2D, albedoTexture);
}

void FrameBuffer::bindNormal(unsigned int slot)
{
    mp_context->glActiveTexture(GL_TEXTURE0 + slot);
    mp_context->glBindTexture(GL_TEXTURE_2D, normalTexture);
}

void FrameBuffer::bindPosition(unsigned int slot)
{
    mp_context->glActiveTexture(GL_TEXTURE0 + slot);
    mp_context->glBindTexture(GL_TEXTURE_2D, positionTexture);
}

void FrameBuffer::bindDepth(unsigned int slot)
{
    mp_context->glActiveTexture(GL_TEXTURE0 + slot);
    mp_context->glBindTexture(GL_TEXTURE_2D, depthTexture);
}










