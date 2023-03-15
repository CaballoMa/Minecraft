#ifndef SNOW_H
#define SNOW_H
#include "drawable.h"
#include <glm_includes.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>


class Snow: public InstancedDrawable
{
public:
    Snow(OpenGLContext* context) : InstancedDrawable(context){}
    virtual ~Snow(){}
    void createVBOdata() override;
    void createInstancedVBOdata(std::vector<glm::vec3> &offsets, std::vector<glm::vec3> &colors) override;
};

#endif // SNOW_H
