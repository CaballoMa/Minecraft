#include "mygl.h"
#include <glm_includes.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>

#define DEPTH_WIDTH  1024
#define DEPTH_HEIGHT 1024

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_worldAxes(this),
      m_progLambert(this), m_progFlat(this),m_progPost(this), m_quad(this), m_progSnow(this), m_progRain(this),
      m_terrain(this),m_player(glm::vec3(40.f, 170.f, 115.f), m_terrain),
      m_terrain_texture(this),m_Time(0), m_WeatherTime(55), depth_fb(this,this->width(), this->height(), this->devicePixelRatio()),
      screen_quad(this), screen_quad2(this),m_shadowPass(this), m_lightShadingPass(this),
      m_GPass(this),post_fb(this,this->width(), this->height(), this->devicePixelRatio()),
      gpass_fb(this,this->width(), this->height(), this->devicePixelRatio()), cloud_noise(this), sky_shader(this), sky_depth_fb(this,this->width(), this->height(), this->devicePixelRatio())
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible


    previous_time = QDateTime::currentMSecsSinceEpoch();

    // L-System Initialize
    m_terrain.initializeLSystem(m_terrain.lsystem);
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}


void MyGL::moveMouseToCenter() {
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);
    //glClearColor(0.2, 0.3, 1, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of the world axes
    m_worldAxes.createVBOdata();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    m_progPost.create(":/glsl/postEffect.vert.glsl", ":/glsl/postEffect.frag.glsl");
    // Create shadow pass
    m_shadowPass.create(":/glsl/shadowPass.vert.glsl", ":/glsl/shadowPass.frag.glsl");
    // Create quad
    m_quad.create(":/glsl/quad.vert.glsl", ":/glsl/quad.frag.glsl");

    m_lightShadingPass.create(":/glsl/lightPass.vert.glsl", ":/glsl/lightPass.frag.glsl");
    m_GPass.create(":/glsl/GPass.vert.glsl", ":/glsl/GPass.frag.glsl");
    m_progSnow.create(":/glsl/snow.vert.glsl", ":/glsl/snow.frag.glsl");
    m_progSnow.setModelMatrix(glm::mat4());
    m_progRain.create(":/glsl/rain.vert.glsl", ":/glsl/rain.frag.glsl");
    m_progRain.setModelMatrix(glm::mat4());
    sky_shader.create(":/glsl/sky_lut.vert.glsl", ":/glsl/sky_lut.frag.glsl");

    //create terrain texture
    const char* path = ":/textures/minecraft_textures_all.png";
    m_terrain_texture.create(path);
    m_terrain_texture.load(TEXTURE_SLOT_TERRAIN);


    depth_fb.create();
    //depth_fb.createDepthFB();

    gpass_fb.createGBuffer();

    screen_quad.createVBOdata();
    screen_quad2.createVBOdata();
    //m_terrain.CreateTestScene();

    m_terrain.m_geomRain.createVBOdata();
    m_terrain.m_geomSnow.createVBOdata();

    post_fb.create();

    sky_depth_fb.create();
    setupSkyShader();

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.

    //cloud_noise.create(":/textures/noise.png");
    //cloud_noise.loadNoise(TEXTURE_SLOT_NOISE);
    glBindVertexArray(vao);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}

void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    m_shadowPass.setLightViewProjMatrix(createLightViewProj());
    m_shadowPass.setViewProjMatrix(viewproj);

    m_lightShadingPass.setViewProjMatrix(viewproj);
    m_lightShadingPass.setLightViewProjMatrix(createLightViewProj());

    m_GPass.setViewProjMatrix(viewproj);

    depth_fb.resize(w, h, this->devicePixelRatio());
    depth_fb.destroy();
    depth_fb.create();
    //depth_fb.createDepthFB();

    gpass_fb.resize(w, h, this->devicePixelRatio());
    gpass_fb.destroy();
    gpass_fb.createGBuffer();

    post_fb.resize(w, h, this->devicePixelRatio());
    post_fb.destroy();
    //post_fb.createGBuffer();
    post_fb.create();

    m_progLambert.setCamPos(m_player.mcr_camera.mcr_position);

    m_progPost.setDimensions(glm::ivec2(w * this->devicePixelRatio(), h * this->devicePixelRatio()));


    sky_shader.setDimensions(glm::ivec2(w * this->devicePixelRatio(), h * this->devicePixelRatio()));
    auto prog = sky_shader.prog;
    auto InverseViewProj = glm::inverse(viewproj);
    glUniformMatrix4fv(glGetUniformLocation(prog, "InverseViewProj"), 1, GL_FALSE, &InverseViewProj[0][0]);

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data
    m_player.mcr_prevposition = m_player.mcr_position;
    auto curr_time = QDateTime::currentMSecsSinceEpoch();
    auto dif = curr_time - previous_time;
    auto second = dif * 0.001;
    previous_time = curr_time;
    m_player.tick(second, m_inputs);

}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {

    //generate terrain data
    m_Time++;
    glm::vec3 playerPos = m_player.mcr_position;
    m_terrain.terrainCheck(playerPos, m_player.mcr_prevposition);

//    if(m_WeatherTime > 60)
//    {

////        if(rand() % 10 > 5)
////        {
////            m_terrain.create_snow(playerPos.x - 80,playerPos.x + 80,
////                                  playerPos.z - 80,playerPos.z + 80);
////            m_WeatherTime = 0;
////        }
////        else
////        {
////            m_terrain.create_rain(playerPos.x - 80,playerPos.x + 80,
////                                  playerPos.z - 80,playerPos.z + 80);
////            m_WeatherTime = 0;
////        }

//    }
    post_fb.bindFrameBuffer();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_progLambert.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLambert.setTime(m_Time);
    m_progLambert.setCamPos(m_player.mcr_camera.mcr_position);
    m_progLambert.setPlayerPos(m_player.mcr_camera.mcr_position);
    m_progSnow.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progRain.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_WeatherTime += 0.01f;
    m_progPost.setTime(m_Time);
    m_progSnow.setTime(m_Time);
    m_progRain.setTime(m_Time);

    renderTerrain(&m_progLambert);
    skyPass();


    glm::vec3 pos = glm::floor(m_player.mcr_camera.mcr_position);
    BlockType currBlock = EMPTY;
    if(m_terrain.hasChunkAt(pos.x, pos.z))
    {
        currBlock = m_terrain.getBlockAt(pos);
        if(m_terrain.getChunkAt(pos.x, pos.z)->rain && m_WeatherTime > 1.5f)
        {
            m_progPost.setPostType(3);
        }
        else if(m_terrain.getChunkAt(pos.x, pos.z)->snow && m_WeatherTime > 3)
        {
            m_progPost.setPostType(4);
        }
        else if(currBlock == WATER)
        {
            m_progPost.setPostType(1);
        }
        else if(currBlock == LAVA)
        {
            m_progPost.setPostType(2);
        }
        else
        {
            m_progPost.setPostType(0);
        }
    }


    debugPass();

    if(m_terrain.generateSnow)
    {
        if(m_WeatherTime < 2 && !m_terrain.snowSmallScale)
        {
            m_terrain.create_snow(playerPos.x - 80,playerPos.x + 80,
                                  playerPos.z - 80,playerPos.z + 80, playerPos.y, 9);
            m_terrain.snowSmallScale = true;
        }
        else if(m_WeatherTime < 5 && !m_terrain.snowMidScale)
        {
            m_terrain.create_snow(playerPos.x - 80,playerPos.x + 80,
                                  playerPos.z - 80,playerPos.z + 80, playerPos.y, 4);
             m_terrain.snowMidScale = true;
        }
        else if(m_WeatherTime > 8)
        {
            m_terrain.create_snow(playerPos.x - 80,playerPos.x + 80,
                                  playerPos.z - 80,playerPos.z + 80, playerPos.y, 2);
            m_terrain.generateSnow = false;
        }

    }
    m_terrain.draw_snow(&m_progSnow);
    m_terrain.draw_rain(&m_progRain);



}

void MyGL::shadowPass()
{
    depth_fb.bindFrameBuffer();
    glClearColor(1,1,1,1);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shadowPass.setLightViewProjMatrix(createLightViewProj());
    m_shadowPass.setViewProjMatrix(m_player.mcr_camera.getViewProj());//createLightViewProj());
    renderTerrain(&m_shadowPass);
//        m_progLambert.setViewProjMatrix(m_player.mcr_camera.getViewProj());
//        m_progLambert.setTime(m_Time);
//        m_Time++;
//        renderTerrain(&m_progLambert);
}

void MyGL::geometryPass()
{
    gpass_fb.bindFrameBuffer();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    m_GPass.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    //m_GPass.setModelMatrix(glm::mat4());
    renderTerrain(&m_GPass);
}

void MyGL::lightPass()
{
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
    glViewport(0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_lightShadingPass.setLightViewProjMatrix(createLightViewProj());
    m_lightShadingPass.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_lightShadingPass.setModelMatrix(glm::mat4());
    depth_fb.bindToTextureSlot(TEXTURE_SLOT_DEPTHMAP);
    m_lightShadingPass.setShadowSampler(TEXTURE_SLOT_DEPTHMAP);
    m_lightShadingPass.setCamPos(m_player.mcr_camera.mcr_position);
    m_lightShadingPass.setLightPos(light_pos);
    m_lightShadingPass.setTime(m_Time);
    m_Time++;
    renderTerrain(&m_lightShadingPass);

    glDisable(GL_BLEND);
}

void MyGL::debugPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
    glViewport(0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    post_fb.bindToTextureSlot(TEXTURE_SLOT_POST);
    m_progPost.drawArray(screen_quad2, TEXTURE_SLOT_POST);
}

void MyGL::skyPass()
{
    updateLightDir();
    glm::mat4 InverseViewMatrix         = glm::inverse(m_player.mcr_camera.getView());
    glm::mat4 InverseProjectionMatrix   = glm::inverse(m_player.mcr_camera.getProj());
    glm::mat4 InverseViewProj           = glm::inverse(m_player.mcr_camera.getViewProj());

    auto prog = sky_shader.prog;
    glUseProgram(prog);
    glUniformMatrix4fv(glGetUniformLocation(prog, "InverseViewMatrix"), 1, GL_FALSE, &InverseViewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(prog, "InverseProjectionMatrix"), 1, GL_FALSE, &InverseProjectionMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(prog, "InverseViewProj"), 1, GL_FALSE, &InverseViewProj[0][0]);
    glUniform1i(glGetUniformLocation(prog, "u_sampler"), TEXTURE_SLOT_SKYDEPTH);
    glUniform1i(glGetUniformLocation(prog, "u_albedoSampler"), TEXTURE_SLOT_ALBEDO);
    sky_shader.drawSky(screen_quad);
}

void MyGL::renderPostEffect()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
    glViewport(0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    depth_fb.bindToTextureSlot(TEXTURE_SLOT_DEPTHMAP);
    m_quad.drawArray(screen_quad, TEXTURE_SLOT_DEPTHMAP);
}

void MyGL::renderTerrain(ShaderProgram* program) {

    m_terrain_texture.bind(TEXTURE_SLOT_TERRAIN);
    program->setTerrainSampler(TEXTURE_SLOT_TERRAIN);

    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    m_terrain.draw(chunk.x - 96, chunk.x + 96,
                   chunk.y - 96, chunk.y + 96, program);
}

void MyGL::keyReleaseEvent(QKeyEvent *e)
{

    if(e->key() == Qt::Key_W){
        m_inputs.wPressed = false;
    }else if(e->key() == Qt::Key_S){
        m_inputs.sPressed = false;
    }else if(e->key() == Qt::Key_A){
        m_inputs.aPressed = false;
    }else if(e->key() == Qt::Key_D){
        m_inputs.dPressed = false;
    }else if(e->key() == Qt::Key_Space){
        m_inputs.spacePressed = false;
        //m_player.canjump = true;
    }else if(e->key() == Qt::Key_Q){
        m_inputs.qPressed = false;
    }else if(e->key() == Qt::Key_E){
        m_inputs.ePressed = false;
    }
    else if(e->key() == Qt::Key_L)
    {
        glm::vec3 playerPos = m_player.mcr_position;
        m_terrain.create_rain(playerPos.x - 80,playerPos.x + 80,
                              playerPos.z - 80,playerPos.z + 80, playerPos.y);
        m_WeatherTime = 0;
    }
    else if(e->key() == Qt::Key_K)
    {
        m_terrain.generateSnow = true;
        m_WeatherTime = 0;
    }
}

void MyGL::keyPressEvent(QKeyEvent *e) {

    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead

    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    }else if(e->key() == Qt::Key_W){
        m_inputs.wPressed = true;
    }else if(e->key() == Qt::Key_S){
        m_inputs.sPressed = true;
    }else if(e->key() == Qt::Key_A){
        m_inputs.aPressed = true;
    }else if(e->key() == Qt::Key_D){
        m_inputs.dPressed = true;
    }else if(e->key() == Qt::Key_Space){
        //m_player.canjump = false;
        m_inputs.spacePressed = true;
    }else if(e->key() == Qt::Key_F){
        m_player.FLY_MODE = !m_player.FLY_MODE;
    }else if(e->key() == Qt::Key_Q){
        m_inputs.qPressed = true;
    }else if(e->key() == Qt::Key_E){
        m_inputs.ePressed = true;
    }else if(e->key() == Qt::Key_G){
        auto v = m_player.getVelocity();
        qDebug() << v.x << " " << v.y << " " << v.z;
    }

}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    // TODO
    QPoint center(width() / 2, height() / 2);
    int x_dif = center.x() - e->pos().x();
    int y_dif = center.y() - e->pos().y();
    m_player.rotateOnUpGlobal(x_dif * 0.2);
    m_player.rotateOnRightLocal(y_dif * 0.2);
    m_inputs.mouseX = x_dif;
    m_inputs.mouseY = y_dif;
    moveMouseToCenter();
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    // TODO
    glm::vec3 pos = m_player.mcr_camera.mcr_position;

    glm::vec3 dir = glm::normalize(m_player.getForward()) * 3.f;
    float out_dist;
    glm::ivec3 hit;
    if(m_player.gridMarch(pos, dir, m_terrain, &out_dist, &hit)){
        if(e->button() == Qt::LeftButton){
            if(m_terrain.hasChunkAt(hit.x, hit.z))
            {
                m_terrain.setBlockAt(hit.x, hit.y, hit.z, BlockType::EMPTY);
                uPtr<Chunk>& chunk = m_terrain.getChunkAt(hit.x, hit.z);
                chunk->createVBOdata();
                chunk->loadVBOdata();
                for(const auto& c : chunk->m_neighbors)
                {
                    c.second->createVBOdata();
                    c.second->loadVBOdata();
                }
            }

        }
        else if(e->button() == Qt::RightButton){
            auto p = glm::floor(pos + glm::normalize(m_player.getForward()) * out_dist);
            if(m_terrain.hasChunkAt(hit.x, hit.z))
            {
                m_terrain.setBlockAt(p.x, p.y, p.z, BlockType::STONE);
                uPtr<Chunk>& chunk = m_terrain.getChunkAt(p.x, p.z);
                chunk->createVBOdata();
                chunk->loadVBOdata();
            }
        }
    }
}

glm::mat4 MyGL::createLightViewProj()
{
    int w = this->width() * this->devicePixelRatio();
    int h = this->height() * this->devicePixelRatio();
    //assume light always look at player
    auto target = m_player.mcr_position;
    target = glm::vec3(0,0,0);
    glm::vec3 lpos = glm::vec3(44, 160, 200);
    //target = glm::vec3(0, 140 ,0);
    //glm::ortho(-10, 10, -10, 10, -10, 10);
    auto pers = glm::ortho<float>(-200, 200, -200, 200, 1, 200);// glm::perspective(glm::radians(45.f), static_cast<float>(w/h), 0.1f, 1000.f);
    auto look = glm::lookAt(lpos, target, glm::vec3(0,1,0));
    return pers * look;
}


void MyGL::setupSkyShader()
{
    //setup parameters
    float RayleighScatterCoef           = 1;
    float RayleighExtinctionCoef        = 1;
    float MieScatterCoef                = 1;
    float MieExtinctionCoef             = 1;
    float DistanceScale                 = 1;
    glm::vec3 RayleighSct               = glm::vec3(5.8f, 13.5f, 33.1f) * 0.000001f;
    glm::vec3 MieSct                    = glm::vec3(2.0f, 2.0f , 2.0f)  * 0.00001f;
    glm::mat4 InverseViewMatrix         = glm::inverse(m_player.mcr_camera.getView());
    glm::mat4 InverseProjectionMatrix   = glm::inverse(m_player.mcr_camera.getProj());
    glm::mat4 InverseViewProj           = glm::inverse(m_player.mcr_camera.getViewProj());
    glm::vec3 IncomingLight             = glm::vec3(4, 4, 4);
    glm::vec3 WorldSpaceCameraPos       = m_player.mcr_camera.mcr_position;
    //glm::vec3 lightDir                  = glm::vec3(100, 220, 100) - m_player.mcr_camera.mcr_position;
    glm::vec3 ScatteringR               = RayleighSct * RayleighScatterCoef;
    glm::vec3 ScatteringM               = MieSct  * MieScatterCoef;
    glm::vec3 ExtinctionR               = RayleighSct * RayleighExtinctionCoef;
    glm::vec3 ExtinctionM               = MieSct * MieExtinctionCoef;
    glm::vec2 DensityScaleHeight        = glm::vec2(7994.0f, 1200.0f);
    float PlanetRadius                  = 637.1f;
    float AtmosphereHeight              = 80000.0f;
    float SunIntensity                  = 1;
    float MieG                          = 0.76f;

    //set data to GPU
    auto prog = sky_shader.prog;
    glUseProgram(prog);
    glUniformMatrix4fv(glGetUniformLocation(prog, "InverseViewMatrix"), 1, GL_FALSE, &InverseViewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(prog, "InverseProjectionMatrix"), 1, GL_FALSE, &InverseProjectionMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(prog, "InverseViewProj"), 1, GL_FALSE, &InverseViewProj[0][0]);
    glUniform3fv(glGetUniformLocation(prog, "IncomingLight"), 1, &IncomingLight[0]);
    glUniform3fv(glGetUniformLocation(prog, "WorldSpaceCameraPos"), 1, &WorldSpaceCameraPos[0]);
    //glUniform3fv(glGetUniformLocation(prog, "lightDir"), 1, &lightDir[0]);
    glUniform3fv(glGetUniformLocation(prog, "ScatteringR"), 1, &ScatteringR[0]);
    glUniform3fv(glGetUniformLocation(prog, "ScatteringM"), 1, &ScatteringM[0]);
    glUniform3fv(glGetUniformLocation(prog, "ExtinctionR"), 1, &ExtinctionR[0]);
    glUniform3fv(glGetUniformLocation(prog, "ExtinctionM"), 1, &ExtinctionM[0]);
    glUniform2fv(glGetUniformLocation(prog, "DensityScaleHeight"), 1, &DensityScaleHeight[0]);
    glUniform1f(glGetUniformLocation(prog, "PlanetRadius"), PlanetRadius);
    glUniform1f(glGetUniformLocation(prog, "AtmosphereHeight"), AtmosphereHeight);
    glUniform1f(glGetUniformLocation(prog, "SunIntensity"), SunIntensity);
    glUniform1f(glGetUniformLocation(prog, "MieG"), MieG);

}


void MyGL::updateLightDir()
{
    float y = sunRadius * sin(angle);
    float x = sunRadius * cos(angle);
    sunPosition = glm::vec3(x, y, 0) + m_player.mcr_position;
    glm::vec3 lightDir = sunPosition - m_player.mcr_position;
    auto prog = sky_shader.prog;
    glUseProgram(prog);
    glUniform3fv(glGetUniformLocation(prog, "lightDir"), 1, &lightDir[0]);
    if(lightDir.y > 1.f)
    {
        m_progLambert.setLightDir(glm::vec4(lightDir,1));
    }


    angle += 0.005f;

    if(angle > 4.5){
        angle = 0;
        glm::vec3 IncomingLight = glm::vec3(4, 4, 4);
        glUniform3fv(glGetUniformLocation(prog, "IncomingLight"), 1, &IncomingLight[0]);
    }else if(angle >= 3.14){
        glm::vec3 IncomingLight = glm::vec3(0.01, 0.01, 0.01);
        glUniform3fv(glGetUniformLocation(prog, "IncomingLight"), 1, &IncomingLight[0]);
    }
}









