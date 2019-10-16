#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "levelObject.h"
#include <sp2/scene/particleEmitter.h>
#include "controls.h"


class GrablingRope;
class Spaceship : public LevelObject
{
public:
    Spaceship(sp::P<sp::Node> parent);

    virtual void onFixedUpdate() override;
    virtual void onUpdate(float delta) override;
    
    virtual void onCollision(sp::CollisionInfo& info) override;
    
    void explode();
    
    bool isAlive() { return alive; }
    
    void setIcon(sp::string name);
    void setIndex(int idx);

    sp::P<sp::Node> icon;
    int index;
    Controls* controls = nullptr;
    bool engine_active = false;
    uint32_t server_position_counter = 1;
    uint32_t local_index = 0;
    uint32_t client_id = 0;
private:
    void updateFromClient(sp::Vector2d position, double rotation, sp::Vector2d velocity, double angular_velocity, bool engine_active);
    void activateHook();
    void activateSelfDestruct();

    sp::Vector2d old_velocity;
    sp::P<sp::ParticleEmitter> engine_emitter;
    bool alive = true;
    sp::P<GrablingRope> rope;
};


#endif//SPACESHIP_H
