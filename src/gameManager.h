#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <sp2/scene/node.h>

class Spaceship;
class GameManager : public sp::Node
{
public:
    GameManager(sp::P<sp::Node> parent);

    void joinTeam(uint32_t client_id, uint32_t local_index, int team);
    void respawn(uint32_t client_id, uint32_t local_index);

    int getTeam(uint32_t client_id, uint32_t local_index);

    virtual void onUpdate(float delta) override;
private:
    void updateTeamSizes();

    int team_size[2] = {0, 0};

    class ClientInfo
    {
    public:
        int team = -1;
        sp::P<Spaceship> ship;
        float respawn_delay = 0.0f;
    };

    std::unordered_map<uint64_t, ClientInfo> client_info;
};

#endif//GAME_MANAGER_H
