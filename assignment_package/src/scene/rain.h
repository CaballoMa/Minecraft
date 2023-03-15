#ifndef RAIN_H
#define RAIN_H

#include "drawable.h"
#include <glm_includes.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Rain:public InstancedDrawable
{
public:
    Rain(OpenGLContext* context) : InstancedDrawable(context){}
    virtual ~Rain(){}
    void createVBOdata() override;
    void createInstancedVBOdata(std::vector<glm::vec3> &offsets, std::vector<glm::vec3> &colors) override;
};


#endif // RAIN_H
