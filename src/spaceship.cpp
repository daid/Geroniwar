#include "spaceship.h"
#include "lineNodeBuilder.h"
#include "grablingRope.h"
#include "explosion.h"
#include "main.h"

#include <sp2/random.h>
#include <sp2/tween.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/multiplayer/registry.h>


class SpaceshipReplication : public sp::multiplayer::ReplicationLinkBase
{
public:
    SpaceshipReplication(Spaceship& ship)
    : ship(ship)
    {
    }

    virtual bool isChanged(float time_delta) override
    {
        sp::Vector2d position = ship.getPosition2D();
        double rotation = ship.getRotation2D();

        last_position += last_velocity * double(time_delta);
        sp::Vector2d diff = last_position - position;

        if (diff.dot(diff) < 0.1 * 0.1)
        {
            double angle_diff = sp::angleDifference(last_rotation, rotation);
            if (angle_diff < 1.0)
                return false;
        }

        last_velocity = ship.getLinearVelocity2D();
        last_position = position;
        last_rotation = rotation;
        return true;
    }

    virtual void initialSend(sp::multiplayer::Base& registry, sp::io::DataBuffer& packet) override
    {
        send(registry, packet);
    }

    virtual void send(sp::multiplayer::Base& registry, sp::io::DataBuffer& packet) override
    {
        sp::Vector2d pos = ship.getPosition2D();
        sp::Vector2d velocity = ship.getLinearVelocity2D();
        double rotation = ship.getRotation2D();
        double angular_velocity = ship.getAngularVelocity2D();
        packet.write(ship.server_position_counter);
        packet.write(pos.x, pos.y, velocity.x, velocity.y);
        packet.write(rotation, angular_velocity, ship.engine_active);
    }

    virtual void receive(sp::multiplayer::Base& registry, sp::io::DataBuffer& packet) override
    {
        sp::Vector2d pos;
        sp::Vector2d velocity;
        double rotation;
        double angular_velocity;
        bool engine_active;
        uint32_t position_counter;
        packet.read(position_counter);
        packet.read(pos.x, pos.y, velocity.x, velocity.y);
        packet.read(rotation, angular_velocity, engine_active);
        
        pos += velocity * double(registry.getNetworkDelay());
        if (position_counter != local_position_counter || !ship.controls)
        {
            ship.setPosition(pos);
            ship.setLinearVelocity(velocity);
            ship.setRotation(rotation);
            ship.setAngularVelocity(angular_velocity);
            ship.engine_active = engine_active;
            local_position_counter = position_counter;
        }
    }

private:
    Spaceship& ship;
    sp::Vector2d last_position, last_velocity;
    double last_rotation = 0.0;
    uint32_t local_position_counter = 0;
};


REGISTER_MULTIPLAYER_CLASS(Spaceship);
Spaceship::Spaceship(sp::P<sp::Node> parent)
: LevelObject(parent)
{
    multiplayer.enable();
    multiplayer.addReplicationLink<SpaceshipReplication>(*this);
    multiplayer.replicate(engine_active);
    multiplayer.replicate(&Spaceship::updateFromClient);
    multiplayer.replicate(&Spaceship::activateHook);
    multiplayer.replicate(&Spaceship::activateSelfDestruct);
    multiplayer.replicate(local_index);
    multiplayer.replicate(client_id, [this]() {
        if (client_id == multiplayer_base->getClientId())
            controls = &controller[local_index];
    });
    multiplayer.replicate(alive);
    multiplayer.replicate(render_data.color.r);
    multiplayer.replicate(render_data.color.g);
    multiplayer.replicate(render_data.color.b);
    multiplayer.replicate(render_data.color.a);
    multiplayer.replicate(rope);

    LineNodeBuilder builder;
    builder.loadFrom("ship-a.json", 1.5);
    builder.create(this, LineNodeBuilder::CollisionType::Polygon);

    engine_emitter = new sp::ParticleEmitter(this, 16, sp::ParticleEmitter::Origin::Global);
}

void Spaceship::onFixedUpdate()
{
    float turn_speed = 5;
    float trust_speed = 0.4;

    double angular_velocity = getAngularVelocity2D() * 0.95;
    sp::Vector2d velocity = getLinearVelocity2D();
    velocity += sp::Vector2d(0, -0.15);

    if (alive && controls)
    {
        float turn = controls->left.getValue() - controls->right.getValue();
        angular_velocity += turn * turn_speed;
        
        engine_active = controls->primary_action.get();
        if (engine_active)
        {
            sp::Vector2d trust_vector = sp::Vector2d(0, trust_speed).rotate(getRotation2D());
            velocity += trust_vector;
        }

        if (controls->secondary_action.getDown())
        {
            multiplayer.callOnServer(&Spaceship::activateHook);
        }
        if (controls->self_destruct.getDown())
        {
            multiplayer.callOnServer(&Spaceship::activateSelfDestruct);
        }
        if (multiplayer_base->getClientId() != 0)
        {
            multiplayer.callOnServer(&Spaceship::updateFromClient, getPosition2D(), getRotation2D(), getLinearVelocity2D(), getAngularVelocity2D(), engine_active);
        }
    }

    if (alive && engine_active)
    {
        sp::Vector2f particle_vector = sp::Vector2f(sp::random(-0.2, 0.2), 1).rotate(getRotation2D()) * sp::random(0.9, 1.1);
        sp::ParticleEmitter::Parameters parameters;
        parameters.position.x = -particle_vector.x;
        parameters.position.y = -particle_vector.y;
        parameters.velocity.x = velocity.x + particle_vector.x * -30;
        parameters.velocity.y = velocity.y + particle_vector.y * -30;
        parameters.acceleration = -parameters.velocity;
        //parameters.start_color = sp::Color(1, 1, 0);
        //parameters.end_color = sp::Color(1, 0, 0, 0);
        parameters.start_color = render_data.color;
        parameters.end_color = parameters.start_color;
        parameters.end_color.a = 0;

        parameters.start_size = 1.0;
        parameters.end_size = 4.0;
        parameters.lifetime = 0.3;
        if (engine_emitter)
            engine_emitter->emit(parameters);
    }

    setLinearVelocity(velocity);
    setAngularVelocity(angular_velocity);
    
    if (alive && multiplayer_base->getClientId() == 0)
    {
        double velo_diff = (velocity - old_velocity).length();
        if (velo_diff > 10.0)
        {
            LOG(Debug, "VELO_DIFF", velo_diff);
            explode();
            return;
        }
    }

    old_velocity = velocity;
}

void Spaceship::onUpdate(float delta)
{
    if (icon)
    {
        if (icon->render_data.type != sp::RenderData::Type::None)
        {
            icon->setPosition(getPosition2D() + sp::Vector2d(0, 6));
            icon->render_data.color.a -= delta * 0.3;
            if (icon->render_data.color.a < 0.0)
                icon->render_data.type = sp::RenderData::Type::None;
        }
    }
}

void Spaceship::setIcon(sp::string name)
{
    if (icon)
    {
        icon->render_data.texture = sp::texture_manager.get("gui/icons/" + name + ".png");
        icon->render_data.color.a = 0.8;
        icon->render_data.type = sp::RenderData::Type::Additive;
    }
}

void Spaceship::setTeam(int idx)
{
    this->team = idx;

    switch(idx)
    {
        case 0: render_data.color = sp::Color(0.8, 0.8, 1); break;
        case 1: render_data.color = sp::Color(1, 0.8, 0.8); break;
        default: render_data.color = sp::Color(1, 1, 1); break;
    }
}

void Spaceship::onCollision(sp::CollisionInfo& info)
{
    if (!alive)
        return;
    if (info.force > 0.0 && multiplayer_base->getClientId() == 0)
    {
        sp::Vector2d local_hit_normal = info.normal.rotate(-getRotation2D());
        if (local_hit_normal.y > -0.7)
        {
            LOG(Debug, this, "HIT", info.force, local_hit_normal);
            explode();
        }
    }
}

void Spaceship::updateFromClient(sp::Vector2d position, double rotation, sp::Vector2d velocity, double angular_velocity, bool engine_active)
{
    setPosition(position);
    setRotation(rotation);
    setLinearVelocity(velocity);
    setAngularVelocity(angular_velocity);
    this->engine_active = engine_active;
}

void Spaceship::activateHook()
{
    if (!rope)
    {
        rope = new GrablingRope(getParent());
        rope->setOwner(this);
        rope->setPosition(getPosition2D() + sp::Vector2d(0, -1).rotate(getRotation2D()));
        rope->setLinearVelocity(getLinearVelocity2D() + sp::Vector2d(0, -5).rotate(getRotation2D()));
    }
    else
    {
        rope.destroy();
    }
}

void Spaceship::activateSelfDestruct()
{
    LOG(Debug, "Initiating self destruct for player");
    explode();
}

void Spaceship::explode()
{
    if (!alive)
        return;
    Explosion* e= new Explosion(getParent());
    e->setup(getPosition2D(), getLinearVelocity2D());
    engine_emitter->setParent(getParent());
    engine_emitter->auto_destroy = true;

    render_data.color.a = 0.5;
    alive = false;
}
