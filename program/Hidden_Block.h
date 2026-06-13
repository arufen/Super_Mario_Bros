#pragma once
#include "IBlock.h"
#include "Collidable.h"

class HiddenBlock : public IBlock, public Collidable
{
public:
    HiddenBlock(Float2 startPos, int emptyBlockTextureHandle);

    void Update() override;
    void Render(Camera camera) override;

    // Custom Collision handlers
    void OnHitBottom(RigidBody& player) override;
    void OnHitTop(RigidBody& player) override;
    //void OnHitSide(RigidBody& player) override;

    Float2 GetPos() const override { return pos; }
    bool IsActive() const override { return active; }
    Image GetSprite() const override { return image; }

private:
    Float2 pos;
    Image image;
    int texEmpty;
    bool isRevealed;
    bool active;

    void PushPlayerOut(RigidBody& player);
};