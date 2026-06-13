#pragma once
#include "IBlock.h"
#include "Collidable.h"
#include <vector>

enum{CLOCKWISE, COUNTERCLOCKWISE};
class FireBar : public IBlock, public Collidable
{
public:
    FireBar();

    void Init(Float2 centerPos, int blockHandle, int fireHandle,int dir, int fireballCount = 6);
    void Update() override;

    // Kept as 'Camera camera' (by value) to match your engine's virtual interface layout safely!
    void Render(Camera camera) override;

    void OnHitBottom(RigidBody& player) override {}

    Float2 GetPos() const override { return pos; }
    Image GetSprite() const override { return image; }

    struct Fireball : public Collidable {
        float x, y;
    };
    std::vector<Fireball>& GetFireballs() { return fireballs; }
    bool IsActive() const { return active; }

    int rotation_dir;

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