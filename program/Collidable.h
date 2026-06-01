#pragma once

#pragma once
#include "Collider.h"

class Mario; // forward declare so we don't need to include Mario.h

class Collidable
{
public:
    Collider collider;

    virtual void OnHitTop(Mario& mario) {}
    virtual void OnHitBottom(Mario& mario) {}
    virtual void OnHitSide(Mario& mario) {}
    virtual ~Collidable() = default;
};
