#include "levelScene.h"
#include "spaceship.h"

#include <json11/json11.hpp>
#include <sp2/random.h>
#include <sp2/tween.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/gui/loader.h>
#include <sp2/io/cameraCapture.h>
#include <sp2/engine.h>

#include "lineNodeBuilder.h"


LevelScene::LevelScene()
: sp::Scene("LEVEL")
{
    getRoot()->multiplayer.enable();
}

void LevelScene::loadLevel(sp::string name)
{
    {
        LineNodeBuilder builder;
        builder.loadFrom(name + ".json", 2.0);
        sp::P<sp::Node> node = new sp::Node(getRoot());
        builder.create(node, LineNodeBuilder::CollisionType::Chains);
        node->render_data.color = sp::Color(0.8, 1.0, 0.8);
    }
    {
        std::string err;
        json11::Json json = json11::Json::parse(sp::io::ResourceProvider::get(name + ".json")->readAll(), err);

        float tw = json["tilewidth"].number_value();
        float th = json["tileheight"].number_value();
        float offset_x = json["width"].number_value() / 2.0;
        float offset_y = json["height"].number_value() / 2.0;

        view_range.x = offset_x;
        view_range.y = offset_y;
    }
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
