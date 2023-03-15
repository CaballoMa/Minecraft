#pragma once

#include "drawable.h"
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Quad2 : public Drawable
{
public:
    Quad2(OpenGLContext* Context);
    void createVBOdata() override;

private:
    OpenGLContext* context;
};
