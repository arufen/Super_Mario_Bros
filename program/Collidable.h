#pragma once

#include "Collider.h"
#include "Camera.h"

//Debug
#include "Camera.h"


class RigidBody; 

enum class PhysicsLayer
{
    DEFAULT,
    ENEMY,
    ITEM,
    PLAYER,
};

class Collidable
{
public:
    Collider collider;
    bool isTrigger = false;

    //to make sure enemy is not colliding with collectable item
    PhysicsLayer layer = PhysicsLayer::DEFAULT;

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
