#include "camera.h"
#include "spaceship.h"
#include "levelScene.h"

#include <numeric>

Camera::Camera(sp::P<sp::Node> parent)
: sp::Camera(parent)
{
    setOrtographic(sp::Vector2d(80, 60));
}

void Camera::onUpdate(float delta)
{
    std::vector<sp::Vector2d> positions;
    for(auto node : getParent()->getChildren())
    {
        sp::P<Spaceship> ship = node;
        if (ship && ship->controls)
            positions.push_back(ship->getPosition2D());
    }
    if (positions.size() == 0)
        return;
    sp::P<LevelScene> level_scene = sp::Scene::get("LEVEL");
    sp::Vector2d view_range = level_scene->view_range;

    auto center = std::accumulate(positions.begin(), positions.end(), sp::Vector2d(0, 0)) / double(positions.size());
    sp::Vector2d size(80, 60);
    center.x = std::max(center.x, -view_range.x + size.x);
    center.x = std::min(center.x, view_range.x - size.x);
    center.y = std::max(center.y, -view_range.y + size.y);
    center.y = std::min(center.y, view_range.y - size.y);
    for(auto p : positions)
    {
        p.x = std::max(p.x, -view_range.x + 25.0);
        p.x = std::min(p.x, view_range.x - 25.0);
        p.y = std::max(p.y, -view_range.y + 25.0);
        p.y = std::min(p.y, view_range.y - 25.0);
        size.x = std::max(size.x, std::abs(p.x - center.x) + 25.0);
        size.y = std::max(size.y, std::abs(p.y - center.y) + 25.0);
    }
    setPosition(center);
    setOrtographic(size);
}
