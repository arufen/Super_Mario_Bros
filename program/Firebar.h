#pragma once
#include "IBlock.h"
#include "Collidable.h"
#include <vector>


class Fireball : public Collidable {
public:
    float x, y;
    Fireball() {
        x = 0;
        y = 0;
        isTrigger = true;            // Allow Mario to overlap cleanly without getting physically blocked
        layer = PhysicsLayer::ENEMY; // Tag it properly for your physics layers
    }
    virtual void OnHitSide(RigidBody& mario) override;
};

//enum{CLOCKWISE, COUNTERCLOCKWISE};
class FireBar : public IBlock, public Collidable
{
public:
    FireBar();

    void Init(Float2 centerPos, int blockHandle, int fireHandle, RotationDir dir, int fireballCount = 6,float startAngle = 0.0f);
    void Update() override;

    // Kept as 'Camera camera' (by value) to match your engine's virtual interface layout safely!
    void Render(Camera camera) override;

    Float2 GetPos() const override { return pos; }
    Image GetSprite() const override { return image; }

  
  
    std::vector<Fireball>& GetFireballs() { return fireballs; }
    bool IsActive() const { return active; }

    RotationDir rotation_dir;

private:
    Float2 pos;
    Image image;
    bool active = true;

    int fireGraphHandle;
    int fireCount;
    float currentAngle;
    float rotationSpeed;
    float distanceBetweenFire;

    std::vector<Fireball> fireballs;
    Animation fireAnimation;
};