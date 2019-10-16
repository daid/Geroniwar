#ifndef LEVEL_SCENE_H
#define LEVEL_SCENE_H

#include <sp2/scene/scene.h>
#include <sp2/scene/camera.h>
#include <sp2/math/rect.h>
#include <sp2/graphics/gui/widget/widget.h>
#include "controls.h"
#include <vector>

class Spaceship;
class LevelScene : public sp::Scene
{
public:
    LevelScene();
    
    void loadLevel(sp::string name);
    
    virtual void onFixedUpdate() override;
    virtual void onUpdate(float delta) override;
    virtual void onEnable() override;
    virtual void onDisable() override;
    void doCameraShake() { shake = 15; }

private:
    void levelFinished();
    void levelFailed();
    void earnTrophyA(int flags);
    void earnTrophyB(int flags);
    void exitLevel();

    sp::PList<Spaceship> players;
    sp::PList<sp::Node> target_objects;
    sp::P<sp::Camera> camera;
    int end_level_countdown;
    int shake = 0;
    bool level_already_finished;
    int trophy_earned_flags;
    sp::string level_name;
};

#endif//LEVEL_SCENE_H
