#pragma once

#include "Collider.h"
#include "Camera.h"

//Debug
#include "Camera.h"


class RigidBody; 

class Collidable
{
public:
    Collider collider;
    bool isTrigger = false;

    virtual void OnHitTop(RigidBody& mario) {}
    virtual void OnHitBottom(RigidBody& mario) {}
    virtual void OnHitSide(RigidBody& mario) {}
    virtual ~Collidable() = default;

    //debug
    void Render(Camera& camera)
    {
        camera.GlobalRenderBox(collider.x, collider.y, collider.x + collider.width, collider.y + collider.height, GetColor(255, 255, 255), 0);
    }
};
