#include "rain.h"
#include <glm_includes.h>
#include <iostream>

static const int IDX_COUNT = 36;
static const int VERT_COUNT = 24;

static const int snow_IDX_COUNT = 30;
static const int snow_VERT_COUNT = 20;

void createRainVertexPositions(glm::vec4 (&cub_vert_pos)[VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 20.0f/30.0f, 1.0f/30.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 0.0f, 1.0f/30.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 1.0f/30.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 20.0f/30.0f, 1.0f/30.0f, 1.0f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 20.0f/30.0f, 0.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 0.0f, 0.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 0.0f, 1.0f/30.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 20.0f/30.0f, 1.0f/30.0f, 1.0f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec4(0.0f, 20.0f/30.0f, 1.0f/30.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 1.0f/30.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 20.0f/30.0f, 0.0f, 1.0f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec4(0.0f, 20.0f/30.0f, 0.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 0.0f, 0.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 20.0f/30.0f, 0.0f, 1.0f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 20.0f/30.0f, 0.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 20.0f/30.0f, 1.0f/30.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 20.0f/30.0f, 1.0f/30.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 20.0f/30.0f, 0.0f, 1.0f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 0.0f, 1.0f/30.0f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(1.0f/30.0f, 0.0f, 0.0f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.0f, 0.0f, 1.0f/30.0f, 1.0f);
}


void createRainVertexNormals(glm::vec4 (&cub_vert_nor)[VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,0,1,0);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(1,0,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(-1,0,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,0,-1,0);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,1,0,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,-1,0,0);
    }
}

void createRainIndices(GLuint (&cub_idx)[IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Rain::createVBOdata()
{
    GLuint idxes[IDX_COUNT];
    glm::vec4 verPos[VERT_COUNT];
    glm::vec4 verNor[VERT_COUNT];

    createRainVertexPositions(verPos);
    createRainVertexNormals(verNor);
    createRainIndices(idxes);

    m_count = IDX_COUNT;

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, IDX_COUNT * sizeof(GLuint), idxes, GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, VERT_COUNT * sizeof(glm::vec4), verPos, GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, VERT_COUNT * sizeof(glm::vec4), verNor, GL_STATIC_DRAW);

}


void Rain::createInstancedVBOdata(std::vector<glm::vec3> &offsets, std::vector<glm::vec3> &colors) {
    m_numInstances = offsets.size();

    generateOffsetBuf();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPosOffset);
    mp_context->glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), offsets.data(), GL_STATIC_DRAW);


    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
}
