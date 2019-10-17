#include "camera.h"
#include "spaceship.h"

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
    
    auto center = std::accumulate(positions.begin(), positions.end(), sp::Vector2d(0, 0)) / double(positions.size());
    sp::Vector2d size(80, 60);
    for(auto p : positions)
    {
        size.x = std::max(size.x, std::abs(p.x - center.x) + 15.0);
        size.y = std::max(size.y, std::abs(p.y - center.y) + 15.0);
    }
    setPosition(center);
    setOrtographic(size);
}
