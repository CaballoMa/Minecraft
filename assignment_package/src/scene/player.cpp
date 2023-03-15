#include "player.h"
#include <QString>
#include <iostream>

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
      colli_box(m_position), mcr_camera(m_camera)
{}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    processInputs(input);
    computePhysics(dT, mcr_terrain);
//    if(!onGround)
//        qDebug() << "not on ground";
//    else
//        qDebug() << "on ground";
//   qDebug() << m_forward.x << " " << m_forward.y << " " << m_forward.z;
}

void Player::processInputs(InputBundle &inputs) {
    // TODO: Update the Player's velocity and acceleration based on the
    // state of the inputs.

    if(inputs.wPressed) {
        m_acceleration.z =  Acceleration;
        //Deceleration.z = 2;
    }
    if(inputs.sPressed) {
        m_acceleration.z =  -Acceleration;
        //Deceleration.z = 2;
    }
    if(inputs.aPressed) {
        m_acceleration.x =  -Acceleration;
        //Deceleration.x = 2;
    }
    if(inputs.dPressed) {
        m_acceleration.x =  Acceleration;
        //Deceleration.x = 2;
    }
    if(FLY_MODE){
        if(inputs.qPressed) m_acceleration.y = -Acceleration * 5;
        if(inputs.ePressed) m_acceleration.y = Acceleration * 5;
        if(!inputs.qPressed && !inputs.ePressed) m_acceleration.y = 0;
        if(!inputs.aPressed && !inputs.wPressed && !inputs.dPressed && !inputs.sPressed){
            m_velocity = glm::vec3(0, 0, 0);
        }
    }
    if(!FLY_MODE){
        if(inputs.spacePressed && canjump){
            m_acceleration.y = Acceleration * 50;
            canjump = false;
            onGround = false;
        }
        else
            m_acceleration.y = 0;

        if(!inputs.aPressed && !inputs.wPressed && !inputs.dPressed && !inputs.sPressed){
            m_velocity = glm::vec3(0, m_velocity.y, 0);
        }
    }

    if(!inputs.wPressed && !inputs.sPressed){
        m_acceleration.z = 0;
        //Deceleration.z = 2;
    }
    if(!inputs.aPressed && !inputs.dPressed){
        m_acceleration.x = 0;
        //Deceleration.x = 2;
    }



}


void Player::computePhysics(float dT, const Terrain &terrain) {
    // TODO: Update the Player's position based on its acceleration
    // and velocity, and also perform collision detection.


    if(FLY_MODE){
//        if(m_velocity.z > Deceleration.z * dT){
//            m_velocity.z -= Deceleration.z * dT;
//        }else if(m_velocity.z < -Deceleration.z * dT){
//            m_velocity.z += Deceleration.z * dT;
//        }else{
//            m_velocity.z = 0.f;
//        }

//        if(m_velocity.x > Deceleration.x * dT){
//            m_velocity.x -= Deceleration.x * dT;
//        }else if(m_velocity.x < -Deceleration.x * dT){
//            m_velocity.x += Deceleration.x * dT;
//        }else{
//            m_velocity.x = 0.f;
//        }

//        if(m_velocity.y > Deceleration.y * dT){
//            m_velocity.y -= Deceleration.y * dT;
//        }else if(m_velocity.y < -Deceleration.y * dT){
//            m_velocity.y += Deceleration.y * dT;
//        }else{
//            m_velocity.y = 0.f;
//        }

        m_velocity += glm::normalize(m_forward) * m_acceleration.z * dT
                    + glm::normalize(m_right)   * m_acceleration.x * dT
                    + glm::normalize(m_up)      * m_acceleration.y * dT;




        clampSpeed();
        //moveAlongVector(m_velocity * dT);
        moveForwardGlobal(m_velocity.z * dT);
        moveUpGlobal(m_velocity.y * dT);
        moveRightGlobal(m_velocity.x * dT);
    }else{

        //qDebug() << glm::length(m_velocity);
        auto forw = glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z));
        auto righ = glm::normalize(glm::vec3(m_right.x, 0, m_right.z));
        auto uppp = glm::vec3(0,1,0);

        //add gravity
        if(!onGround){
            if (mcr_terrain.hasChunkAt(this->mcr_position.x, this->mcr_position.z))
            {
                if (mcr_terrain.getBlockAt(m_position.x, m_position.y, m_position.z) == WATER)
                {
                    m_velocity *= gravity * 0.667f * glm::vec3(0, -1, 0) * dT;
                } else {
                    m_velocity += gravity * glm::vec3(0, -1, 0) * dT;
                }
            }
        }else{
            m_velocity.y = 0.0f;
        }


        //decrease speed
//        if(m_velocity.z > Deceleration.z * dT){
//            //m_velocity.z += m_forward.z * Deceleration.z * dT + m_right.x * Deceleration.x * dT;
//            m_velocity.z -= Deceleration.z * dT;
//        }else if(m_velocity.z < -Deceleration.z * dT){
//            //m_velocity.z += m_forward.z * -Deceleration.z * dT + m_right.x * -Deceleration.x * dT;
//            m_velocity.z += Deceleration.z * dT;
//        }else{
//            m_velocity.z = 0;
//        }
//        if(m_velocity.x > Deceleration.x * dT){
//            m_velocity.x -= Deceleration.x * dT;
//        }else if(m_velocity.x < -Deceleration.x * dT){
//            m_velocity.x += Deceleration.x * dT;
//        }else{
//            m_velocity.x = 0;
//        }


        m_velocity += forw * m_acceleration.z * dT
                    + righ * m_acceleration.x * dT
                    + uppp * m_acceleration.y * dT;
        if (mcr_terrain.hasChunkAt(this->mcr_position.x, this->mcr_position.z))
        {
            if (mcr_terrain.getBlockAt(m_position.x, m_position.y, m_position.z) == WATER || mcr_terrain.getBlockAt(m_position.x, m_position.y, m_position.z) == LAVA)
            {
                m_velocity *= 0.667;
            }
        }
        clampSpeed();

        float min_dist;
        glm::ivec3 hitBlock;
        if(intersect(glm::vec3(0, 0, m_velocity.z) * dT, min_dist, hitBlock, terrain)){
            //moveForwardLocal(min_dist);
            //moveAlongVector(glm::normalize(glm::vec3(0, 0, m_velocity.z)) * min_dist);
            if(abs(min_dist) > 0.1)
                moveAlongVector(glm::normalize(glm::vec3(0, 0, m_velocity.z)) * min_dist);
            m_velocity.z = 0;

        }else{
            moveForwardGlobal(m_velocity.z * dT);
        }
        if(intersect(glm::vec3(0, m_velocity.y, 0) * dT, min_dist, hitBlock, terrain)){
            //moveUpLocal(min_dist);
            if(abs(min_dist) > 0.1)
                moveAlongVector(glm::normalize(glm::vec3(0, m_velocity.y, 0)) * min_dist);
            if(m_velocity.y <= 0) {
                onGround = true;
                canjump = true;
            }
            m_velocity.y = 0.f;


        }else{
            onGround = false;
            moveUpGlobal(m_velocity.y * dT);
        }
        if(intersect(glm::vec3(m_velocity.x, 0, 0) * dT, min_dist, hitBlock, terrain)){
            //moveRightLocal(min_dist);
            if(abs(min_dist) > 0.1)
                moveAlongVector(glm::normalize(glm::vec3(m_velocity.x, 0, 0)) * min_dist);
            m_velocity.x = 0;

        }else{
            moveRightGlobal(m_velocity.x * dT);
        }

    }

    colli_box.update(m_position);


}

bool Player::intersect(glm::vec3 velocity, float &dist, glm::ivec3 &hitBlock, const Terrain &terrain){
    bool res = false;
    float out_dist_res = FLT_MAX; glm::ivec3 out_blockHit_res;
    for(unsigned int i = 0; i < colli_box.points.size(); i++){
        float out_dist; glm::ivec3 out_blockHit;
        if(gridMarch(colli_box.points[i], velocity, terrain, &out_dist, &out_blockHit)){
            res = true;
            out_blockHit_res = out_dist_res > out_dist? out_blockHit : out_blockHit_res;
            out_dist_res = out_dist_res >  out_dist? out_dist : out_dist_res;
        }
    }
    if(res){
        dist = out_dist_res;
        hitBlock = out_blockHit_res;
        if (terrain.hasChunkAt(out_blockHit_res.x, out_blockHit_res.z))
        {
            if(terrain.getBlockAt(glm::vec3(out_blockHit_res.x, out_blockHit_res.y,out_blockHit_res.z)) == WATER){
                res = false;
            }
        }
    }
    return res;
}

bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit) {
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if(axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if(interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return
        // curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        if(cellType != EMPTY) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}


void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);

}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}

void Player::clampSpeed()
{
    if(m_velocity.x > max_speed) m_velocity.x = max_speed;
    if(m_velocity.x < -max_speed) m_velocity.x = -max_speed;
    if(m_velocity.y > max_speed) m_velocity.y = max_speed;
    if(m_velocity.y < -max_speed) m_velocity.y = -max_speed;
    if(m_velocity.z > max_speed) m_velocity.z = max_speed;
    if(m_velocity.z < -max_speed) m_velocity.z = -max_speed;
}
