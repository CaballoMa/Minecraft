#include "snow.h"

#include "cube.h"
#include <glm_includes.h>
#include <iostream>

static const int IDX_COUNT = 30;
static const int VERT_COUNT = 20;



//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createface(glm::vec4 (&face_vert_pos)[4], glm::vec4 (&face_vert_nor)[4], GLuint (&cub_idx)[6]){
    face_vert_pos[0] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    face_vert_pos[1] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    face_vert_pos[2] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    face_vert_pos[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    face_vert_nor[0] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    face_vert_nor[1] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    face_vert_nor[2] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    face_vert_nor[3] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    cub_idx[0] = 0;
    cub_idx[1] = 1;
    cub_idx[2] = 2;
    cub_idx[3] = 0;
    cub_idx[4] = 2;
    cub_idx[5] = 3;
}
void createSnowVertexPositions(glm::vec4 (&snow_vert_pos)[VERT_COUNT])
{
    int idx = 0;
    snow_vert_pos[idx++] = glm::vec4(1.0f/16.0f, 1.0f/16.0f, 0.0f, 1.0f);
    //LR
    snow_vert_pos[idx++] = glm::vec4(1.0f/16.0f, 2.0f/16.0f, 0.0f, 1.0f);
    //LL
    snow_vert_pos[idx++] = glm::vec4(2.0f/16.0f, 2.0f/16.0f, 0.0f, 1.0f);
    //UL
    snow_vert_pos[idx++] = glm::vec4(2.0f/16.0f, 1.0f/16.0f, 0.0f, 1.0f);

}


void createSnowVertexNormals(glm::vec4 (&snow_vert_nor)[VERT_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 4; i++)
    {
        snow_vert_nor[idx++] = glm::vec4(0,0,-1,0);
    }


}

void createSnowIndices(GLuint (&cub_idx)[IDX_COUNT])
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

void Snow::createVBOdata()
{
    GLuint sph_idx[IDX_COUNT];
    glm::vec4 sph_vert_pos[VERT_COUNT];
    glm::vec4 sph_vert_nor[VERT_COUNT];

    createSnowVertexPositions(sph_vert_pos);
    createSnowVertexNormals(sph_vert_nor);
    createSnowIndices(sph_idx);

    GLuint s_idx[6];
    glm::vec4 s_vert_pos[4];
    glm::vec4 s_vert_nor[4];
    createface(s_vert_pos,s_vert_nor,s_idx);


    m_count = IDX_COUNT;
    //m_count = 6;

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.

    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, IDX_COUNT * sizeof(GLuint), sph_idx, GL_STATIC_DRAW);

    //mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), s_idx, GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, VERT_COUNT * sizeof(glm::vec4), sph_vert_pos, GL_STATIC_DRAW);
    //mp_context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec4), s_vert_pos, GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, VERT_COUNT * sizeof(glm::vec4), sph_vert_nor, GL_STATIC_DRAW);
    //mp_context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec4), s_vert_nor, GL_STATIC_DRAW);


}


void Snow::createInstancedVBOdata(std::vector<glm::vec3> &offsets, std::vector<glm::vec3> &colors) {
    m_numInstances = offsets.size();

    generateOffsetBuf();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPosOffset);
    mp_context->glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), offsets.data(), GL_STATIC_DRAW);


    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
}
