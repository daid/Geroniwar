#include <sp2/engine.h>
#include <sp2/window.h>
#include <sp2/logging.h>
#include <sp2/io/directoryResourceProvider.h>
#include <sp2/io/zipResourceProvider.h>
#include <sp2/graphics/gui/scene.h>
#include <sp2/graphics/gui/theme.h>
#include <sp2/graphics/scene/graphicslayer.h>
#include <sp2/graphics/scene/basicnoderenderpass.h>
#include <sp2/graphics/scene/collisionrenderpass.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/multiplayer/server.h>
#include <sp2/multiplayer/client.h>
#include <sp2/multiplayer/discovery.h>
#include <sp2/scene/scene.h>
#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/io/keybinding.h>
#include <sp2/io/subprocess.h>
#include <sp2/io/http/request.h>

#include "levelScene.h"
#include "spaceship.h"
#include "controls.h"
#include "main.h"
#include "gameManager.h"
#include "camera.h"


Controls controller[2]{{0}, {1}};

sp::P<sp::Window> window;
sp::multiplayer::Base* multiplayer_base;

sp::io::Keybinding escape_key{"exit", "Escape"};

int main(int argc, char** argv)
{
    sp::P<sp::Engine> engine = new sp::Engine();

    //Create resource providers, so we can load things.
    new sp::io::DirectoryResourceProvider("resources");
    new sp::io::ZipResourceProvider(argv[0]);

    //Disable or enable smooth filtering by default, enabling it gives nice smooth looks, but disabling it gives a more pixel art look.
    sp::texture_manager.setDefaultSmoothFiltering(false);

    //Create a window to render on, and our engine.
    window = new sp::Window(4.0/3.0);
#ifndef DEBUG
    window->setFullScreen(true);
    window->hideCursor();
#endif

    sp::gui::Theme::loadTheme("default", "gui/theme/basic.theme.txt");
    new sp::gui::Scene(sp::Vector2d(640, 480));

    sp::P<sp::SceneGraphicsLayer> scene_layer = new sp::SceneGraphicsLayer(1);
    scene_layer->addRenderPass(new sp::BasicNodeRenderPass());
#ifdef DEBUG
    scene_layer->addRenderPass(new sp::CollisionRenderPass());
#endif
    window->addLayer(scene_layer);

    LevelScene* level_scene = new LevelScene();
    Camera* camera = new Camera(level_scene->getRoot());
    level_scene->setDefaultCamera(camera);
    level_scene->loadLevel("level");

    if (argc > 1)
    {
        sp::multiplayer::Discovery discovery("geroniwar");
        discovery.scanSwitchboard("daid.eu", 32032);
        for(auto& info : discovery.getServers())
        {
            if (info.type == sp::multiplayer::Discovery::ServerInfo::Type::Switchboard)
            {
                sp::multiplayer::Client* client = new sp::multiplayer::Client("geroniwar", 0);
                client->connectBySwitchboard(info.switchboard_hostname, info.switchboard_port, info.switchboard_key);
                multiplayer_base = client;
                break;
            }
        }
    }
    else
    {
        sp::multiplayer::Server* server = new sp::multiplayer::Server("geroniwar", 0);
        //server->listen(32032);
        server->listenOnSwitchboard("daid.eu", 32032, "GW", true);
        multiplayer_base = server;

        GameManager* gm = new GameManager(level_scene->getRoot());
        gm->joinTeam(0, 0, 0);
        gm->joinTeam(0, 1, 1);
    }

    engine->run();

    return 0;
}
