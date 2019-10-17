#include "gameManager.h"
#include "spaceship.h"
#include "main.h"

#include <sp2/multiplayer/registry.h>


REGISTER_MULTIPLAYER_CLASS(GameManager);
GameManager::GameManager(sp::P<sp::Node> parent)
: sp::Node(parent)
{
    multiplayer.enable();
    multiplayer.replicate(team_size[0]);
    multiplayer.replicate(team_size[1]);
    multiplayer.replicate(&GameManager::joinTeam);
    multiplayer.replicate(&GameManager::respawn);
}

void GameManager::joinTeam(uint32_t client_id, uint32_t local_index, int team)
{
    if (team < 0)
        team = (team_size[1] < team_size[0]) ? 1 : 0;
    if (team >= 2)
        return;

    uint64_t id = uint64_t(client_id) | (uint64_t(local_index) << 32);
    client_info[id].team = team;
    updateTeamSizes();
}

void GameManager::respawn(uint32_t client_id, uint32_t local_index)
{
    uint64_t id = uint64_t(client_id) | (uint64_t(local_index) << 32);
    auto it = client_info.find(id);
    if (it == client_info.end() || it->second.team < 0)
    {
        joinTeam(client_id, local_index, -1);
        it = client_info.find(id);
    }
    if (it->second.respawn_delay > 0.0f)
        return;

    it->second.ship.destroy();
    it->second.ship = new Spaceship(getParent());
    it->second.ship->local_index = local_index;
    it->second.ship->client_id = client_id;
    it->second.ship->setTeam(it->second.team);
    it->second.respawn_delay = 10.0f;

    if (client_id == 0)
        it->second.ship->controls = &controller[local_index];
}

int GameManager::getTeam(uint32_t client_id, uint32_t local_index)
{
    uint64_t id = uint64_t(client_id) | (uint64_t(local_index) << 32);
    auto it = client_info.find(id);
    if (it == client_info.end())
        return -1;
    return it->second.team;
}

void GameManager::onUpdate(float delta)
{
    for(int n=0; n<2; n++)
    {
        if (controller[n].primary_action.getDown())
            multiplayer.callOnServer(&GameManager::respawn, multiplayer_base->getClientId(), uint32_t(n));
    }

    if (multiplayer_base->getClientId() != 0)
        return;

    uint64_t remove_id = std::numeric_limits<uint64_t>::max();
    for(auto& it : client_info)
    {
        if (!(it.second.ship && it.second.ship->isAlive()))
        {
            it.second.respawn_delay -= delta;
            if (it.second.respawn_delay < -10.0f)
                it.second.ship.destroy();
            if (it.second.respawn_delay < -120.0f)
                remove_id = it.first;
        }
    }
    if (remove_id != std::numeric_limits<uint64_t>::max())
        client_info.erase(remove_id);
}

void GameManager::updateTeamSizes()
{
    team_size[0] = 0;
    team_size[1] = 0;

    for(auto& it : client_info)
    {
        if (it.second.team > -1 && it.second.team < 2)
            team_size[it.second.team] += 1;
    }
}
