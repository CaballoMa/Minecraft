#pragma once

#include "drawable.h"
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Quad : public Drawable
{
public:
    Quad(OpenGLContext* Context);
    void createVBOdata() override;

private:
    OpenGLContext* context;
};
