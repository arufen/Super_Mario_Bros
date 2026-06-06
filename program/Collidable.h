#pragma once

#include "Collider.h"


class RigidBody; // forward declare so we don't need to include Mario.h

class Collidable
{
public:
    Collider collider;

    virtual void OnHitTop(RigidBody& mario) {}
    virtual void OnHitBottom(RigidBody& mario) {}
    virtual void OnHitSide(RigidBody& mario) {}
    virtual ~Collidable() = default;
};
