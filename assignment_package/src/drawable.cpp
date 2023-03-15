#include "drawable.h"
#include <glm_includes.h>

Drawable::Drawable(OpenGLContext* context)
    : m_count(-1), m_opaqueCount(-1), m_translucentCount(-1),m_bufIdx(), m_bufPos(), m_bufNor(), m_bufCol(), m_bufInterleave(),
      m_bufUV(), m_bufTranslucent(), m_idxGenerated(false), m_posGenerated(false),
      m_norGenerated(false), m_colGenerated(false), m_uvGenerated(false), m_interleaveGenerated(false),
      hasBindVBOData(false),hasCreatVBOData(false),
      m_opaqueGenerated(false), m_translucentGenerated(false), mp_context(context)
{}

Drawable::~Drawable()
{}


void Drawable::destroyVBOdata()
{
    mp_context->glDeleteBuffers(1, &m_bufIdx);
    mp_context->glDeleteBuffers(1, &m_bufPos);
    mp_context->glDeleteBuffers(1, &m_bufNor);
    mp_context->glDeleteBuffers(1, &m_bufCol);
    mp_context->glDeleteBuffers(1, &m_bufUV);
    m_idxGenerated = m_posGenerated = m_norGenerated = m_colGenerated = false;
    m_count = -1;
    m_opaqueCount = -1;
    m_translucentCount = -1;
    mp_context->glDeleteBuffers(1, &m_bufOpaque);
    mp_context->glDeleteBuffers(1, &m_bufTranslucent);
    mp_context->glDeleteBuffers(1, &m_bufOpaqueIdx);
    mp_context->glDeleteBuffers(1, &m_bufTranslucentIdx);
    m_opaqueGenerated = m_opaqueIdxGenerated = m_translucentGenerated = m_translucentIdxGenerated = false;
    m_uvGenerated = false;
    hasCreatVBOData = false;
    hasBindVBOData = false;
}

GLenum Drawable::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

int Drawable::elemCount()
{
    return m_count;
}

int Drawable::opaqueElemCount()
{
    return m_opaqueCount;
}

int Drawable::translElemCount()
{
    return m_translucentCount;
}

void Drawable::generateIdx()
{
    m_idxGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &m_bufIdx);
}

void Drawable::generatePos()
{
    m_posGenerated = true;
    // Create a VBO on our GPU and store its handle in bufPos
    mp_context->glGenBuffers(1, &m_bufPos);
}

void Drawable::generateNor()
{
    m_norGenerated = true;
    // Create a VBO on our GPU and store its handle in bufNor
    mp_context->glGenBuffers(1, &m_bufNor);
}

void Drawable::generateCol()
{
    m_colGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mp_context->glGenBuffers(1, &m_bufCol);
}

void Drawable::generateInterleave()
{
    m_interleaveGenerated = true;
    mp_context->glGenBuffers(1, &m_bufInterleave);
}

void Drawable::generateUV()
{
    m_uvGenerated = true;
    mp_context->glGenBuffers(1, &m_bufUV);

}

void Drawable::generateOpaqueIdx(){
    m_opaqueIdxGenerated = true;
    mp_context->glGenBuffers(1, &m_bufOpaqueIdx);
}

void Drawable::generateOpaque(){
    m_opaqueGenerated = true;
    mp_context->glGenBuffers(1, &m_bufOpaque);
}

void Drawable::generateTranslucentIdx()
{
    m_translucentIdxGenerated = true;
    mp_context->glGenBuffers(1, &m_bufTranslucentIdx);
}

void Drawable::generateTranslucent()
{
    m_translucentGenerated = true;
    mp_context->glGenBuffers(1, &m_bufTranslucent);
}

bool Drawable::bindIdx()
{
    if(m_idxGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    }
    return m_idxGenerated;
}

bool Drawable::bindPos()
{
    if(m_posGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    }
    return m_posGenerated;
}

bool Drawable::bindNor()
{
    if(m_norGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    }
    return m_norGenerated;
}

bool Drawable::bindCol()
{
    if(m_colGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    }
    return m_colGenerated;
}

bool Drawable::bindInterleave()
{
    if (m_interleaveGenerated)
    {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleave);
    }
    return m_interleaveGenerated;
}

bool Drawable::bindUV()
{
    if (m_uvGenerated)
    {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufUV);
    }
    return m_uvGenerated;
}

bool Drawable::bindOpaqueIdx()
{
    if(m_opaqueIdxGenerated){
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufOpaqueIdx);
    }
    return m_opaqueIdxGenerated;
}

bool Drawable::bindOpaque()
{
    if(m_opaqueGenerated)
    {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufOpaque);
    }
    return m_opaqueGenerated;
}

bool Drawable::bindTranslucentIdx()
{
    if(m_translucentIdxGenerated)
    {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufTranslucentIdx);
    }
    return m_translucentIdxGenerated;
}

bool Drawable::bindTranslucent()
{
    if(m_translucentGenerated)
    {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufTranslucent);
    }
    return m_translucentGenerated;
}

InstancedDrawable::InstancedDrawable(OpenGLContext *context)
    : Drawable(context), m_numInstances(0), m_bufPosOffset(-1), m_offsetGenerated(false)
{}

InstancedDrawable::~InstancedDrawable(){}

int InstancedDrawable::instanceCount() const {
    return m_numInstances;
}

void InstancedDrawable::generateOffsetBuf() {
    m_offsetGenerated = true;
    mp_context->glGenBuffers(1, &m_bufPosOffset);
}

bool InstancedDrawable::bindOffsetBuf() {
    if(m_offsetGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPosOffset);
    }
    return m_offsetGenerated;
}


void InstancedDrawable::clearOffsetBuf() {
    if(m_offsetGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufPosOffset);
        m_offsetGenerated = false;
    }
}
void InstancedDrawable::clearColorBuf() {
    if(m_colGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufCol);
        m_colGenerated = false;
    }
}
