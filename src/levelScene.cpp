#include "levelScene.h"
#include "spaceship.h"

#include <json11/json11.hpp>
#include <sp2/random.h>
#include <sp2/tween.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/gui/loader.h>
#include <sp2/io/cameraCapture.h>
#include <sp2/engine.h>



LevelScene::LevelScene()
: sp::Scene("LEVEL")
{
    disable();

    getRoot()->multiplayer.enable();
}

void LevelScene::loadLevel(sp::string name)
{
    level_name = name;

    for(auto obj : getRoot()->getChildren())
        obj.destroy();
}

void LevelScene::onFixedUpdate()
{
}

void LevelScene::onUpdate(float delta)
{
}

void LevelScene::onEnable()
{
}

void LevelScene::onDisable()
{
}
