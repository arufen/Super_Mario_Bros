#pragma once
#include "IBlock.h"
#include "Collidable.h"

enum class HiddenItemType {
    MUSHROOM_1UP,
    COIN
};
class HiddenBlock : public IBlock, public Collidable
{
public:
    
    HiddenBlock();

    void Init(Float2 startPos, int emptyBlockTextureHandle, HiddenItemType type);

    void Update() override;
    void Render(Camera camera) override;

    // Custom Collision handlers
    void OnHitBottom(RigidBody& player) override;
    void OnHitTop(RigidBody& player) override;
    //void OnHitSide(RigidBody& player) override;

    Float2 GetPos() const override { return pos; }
    bool IsActive() const override { return active; }
    Image GetSprite() const override { return image; }
    bool IsRevealed() const { return isRevealed; }

    HiddenItemType itemType;

    Animation itemCoinAnim;
    Timer itemCoinAnimationTimer{ 0.5f };

private:
    Float2 pos;
    Image image;
    int texActive;
    bool isRevealed;
    bool active;
    bool itemSpawned;


    void PushPlayerOut(RigidBody& player);
};