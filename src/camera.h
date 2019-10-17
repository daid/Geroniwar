#ifndef CAMERA_H
#define CAMERA_H

#include <sp2/scene/camera.h>

class Camera : public sp::Camera
{
public:
    Camera(sp::P<sp::Node> parent);

    virtual void onUpdate(float delta) override;
};

#endif//CAMERA_H
