#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"

struct collisionBox{
    glm::vec3 origin;
    /*     * - *
     *    *|- *|
     *    |* -|*
     *    * - *
     *    2   3
     *    0   1
     */
    glm::vec3 b0, b1, b2, b3;
    glm::vec3 m0, m1, m2, m3;
    glm::vec3 t0, t1, t2, t3;
    std::vector<glm::vec3> points;
    collisionBox(glm::vec3 pos) : origin(pos){
        b0 = origin + glm::vec3(-0.5, 0, -0.5);
        b1 = origin + glm::vec3( 0.5, 0, -0.5);
        b2 = origin + glm::vec3( 0.5, 0,  0.5);
        b3 = origin + glm::vec3(-0.5, 0,  0.5);

        m0 = b0 + glm::vec3(0, 1, 0);
        m1 = b1 + glm::vec3(0, 1, 0);
        m2 = b2 + glm::vec3(0, 1, 0);
        m3 = b3 + glm::vec3(0, 1, 0);

        t0 = m0 + glm::vec3(0, 1, 0);
        t1 = m1 + glm::vec3(0, 1, 0);
        t2 = m2 + glm::vec3(0, 1, 0);
        t3 = m3 + glm::vec3(0, 1, 0);
        points = {b0,b1,b2,b3,m0,m1,m2,m3,t0,t1,t2,t3};
    }

    void update(glm::vec3 pos){
        this->origin = pos;
        b0 = origin + glm::vec3(-0.5, 0, -0.5);
        b1 = origin + glm::vec3( 0.5, 0, -0.5);
        b2 = origin + glm::vec3( 0.5, 0,  0.5);
        b3 = origin + glm::vec3(-0.5, 0,  0.5);

        m0 = b0 + glm::vec3(0, 1, 0);
        m1 = b1 + glm::vec3(0, 1, 0);
        m2 = b2 + glm::vec3(0, 1, 0);
        m3 = b3 + glm::vec3(0, 1, 0);

        t0 = m0 + glm::vec3(0, 1, 0);
        t1 = m1 + glm::vec3(0, 1, 0);
        t2 = m2 + glm::vec3(0, 1, 0);
        t3 = m3 + glm::vec3(0, 1, 0);
        points = {b0,b1,b2,b3,m0,m1,m2,m3,t0,t1,t2,t3};
    }
};

class Player : public Entity {
private:
    glm::vec3 m_velocity, m_acceleration;

    Camera m_camera;
    const Terrain &mcr_terrain;

    void processInputs(InputBundle &inputs);
    //void processInputs(float dT, InputBundle &inputs);
    void computePhysics(float dT, const Terrain &terrain);

    // for movement
    float Acceleration = 7;
    //float Deceleration = 1;
    float max_speed = 10.f;
    float gravity = 10;
    glm::vec3 Deceleration = glm::vec3(2, 2, 10);

    glm::vec3 forward = glm::vec3();
    glm::vec3 up= glm::vec3();
    glm::vec3 right= glm::vec3();

    void clampSpeed();

    bool onGround = false;
    bool canjump = true;
    collisionBox colli_box;

public:
    // Readonly public reference to our camera
    // for easy access from MyGL
    const Camera& mcr_camera;
    bool FLY_MODE = true;
    //glm::vec3 getForward(){return m_forward;}
//    glm::vec3 getCamForward(){return mcr_camera.getforward()}
    glm::vec3 getVelocity(){return m_velocity;}

    bool gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit) ;
    bool intersect(glm::vec3 velocity, float &dist, glm::ivec3 &hitBlock, const Terrain &terrain);

    Player(glm::vec3 pos, const Terrain &terrain);
    virtual ~Player() override;

    void setCameraWidthHeight(unsigned int w, unsigned int h);

    void tick(float dT, InputBundle &input) override;

    // Player overrides all of Entity's movement
    // functions so that it transforms its camera
    // by the same amount as it transforms itself.
    void moveAlongVector(glm::vec3 dir) override;
    void moveForwardLocal(float amount) override;
    void moveRightLocal(float amount) override;
    void moveUpLocal(float amount) override;
    void moveForwardGlobal(float amount) override;
    void moveRightGlobal(float amount) override;
    void moveUpGlobal(float amount) override;
    void rotateOnForwardLocal(float degrees) override;
    void rotateOnRightLocal(float degrees) override;
    void rotateOnUpLocal(float degrees) override;
    void rotateOnForwardGlobal(float degrees) override;
    void rotateOnRightGlobal(float degrees) override;
    void rotateOnUpGlobal(float degrees) override;

    // For sending the Player's data to the GUI
    // for display
    QString posAsQString() const;
    QString velAsQString() const;
    QString accAsQString() const;
    QString lookAsQString() const;
};
