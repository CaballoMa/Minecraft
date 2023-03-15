#ifndef QUADRANGLE_H
#define QUADRANGLE_H

#include "scene/chunk.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Quadrangle : public Drawable
{
private:
    glm::vec4 vert_pos_nor_col[12];
public:
    Quadrangle(OpenGLContext* context);
    virtual ~Quadrangle(){}
    void create();
    void setInBlock(BlockType t);
    BlockType inBlock;

    void createVBOdata() override;
};

#endif // QUADRANGLE_H
