#include "texture.h"
#include <QImage>
#include <QOpenGLWidget>

Texture::Texture(OpenGLContext *context)
    : context(context), m_textureHandle(-1), m_textureImage(nullptr)
{}

Texture::~Texture()
{}

void Texture::create(const char *texturePath)
{
    context->printGLErrorLog();

    QImage img(texturePath);
    img.convertToFormat(QImage::Format_ARGB32);
    img = img.mirrored();

//    QString filename("../assignment_package/textures/save.png");
//        if(img.save(filename)){
//            qDebug() << "saved";
//        }


    m_textureImage = std::make_shared<QImage>(img);
    context->glGenTextures(1, &m_textureHandle);

    context->printGLErrorLog();
}

void Texture::createNoise(const char *texturePath)
{
    context->printGLErrorLog();

    QImage img(texturePath);
    img.convertToFormat(QImage::Format_RGB32);

    QString filename("../assignment_package/textures/save.png");
    if(img.save(filename)){
        qDebug() << "saved";
    }


    m_textureImage = std::make_shared<QImage>(img);
    context->glGenTextures(1, &m_textureHandle);

    context->printGLErrorLog();
}

void Texture::load(int texSlot = 0)
{
    context->printGLErrorLog();

    context->glActiveTexture(GL_TEXTURE0 + texSlot);
    context->glBindTexture(GL_TEXTURE_2D, m_textureHandle);

    // These parameters need to be set for EVERY texture you create
    // They don't always have to be set to the values given here, but they do need
    // to be set
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                          m_textureImage->width(), m_textureImage->height(),
                          0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, m_textureImage->bits());
    context->printGLErrorLog();
}

void Texture::loadNoise(int texSlot = 0)
{
    context->printGLErrorLog();

    context->glActiveTexture(GL_TEXTURE0 + texSlot);
    context->glBindTexture(GL_TEXTURE_2D, m_textureHandle);

    // These parameters need to be set for EVERY texture you create
    // They don't always have to be set to the values given here, but they do need
    // to be set
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                          m_textureImage->width(), m_textureImage->height(),
                          0, GL_RGB, GL_UNSIGNED_BYTE, m_textureImage->bits());
    context->printGLErrorLog();
}


void Texture::bind(int texSlot = 0)
{
    context->glActiveTexture(GL_TEXTURE0 + texSlot);
    context->glBindTexture(GL_TEXTURE_2D, m_textureHandle);
}


GLuint Texture::createDepthTexture(int width, int height)
{
    // The texture we're going to render to
    context->glGenTextures(1, &m_textureHandle);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    context->glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    // Give an empty image to OpenGL ( the last null )
    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return m_textureHandle;
}



//void Texture::create3D()
//{
//    unsigned int texname;
//    context->glGenTextures(1, &texname);
//    context->glBindTexture(GL_TEXTURE_3D, texname);
//    context->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    context->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    context->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    context->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    context->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
//    context->glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, 256, 256, 256, 0, GL_RGB,
//                 GL_UNSIGNED_BYTE, texels);
//}
















