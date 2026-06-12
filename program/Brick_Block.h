#pragma once
#include "IBlock.h"
#include "Camera.h"

class BrickBlock : public IBlock, public Collidable
{
public:
    BrickBlock();

    void Init(Float2 startPos, int graphHandle);
    void Update() override;
    void Render(Camera camera);

    // Triggered when Mario hits the block from underneath
    virtual void OnHitBottom(RigidBody& player) override;

    Float2 GetPos() const override{ return pos; }
    Image GetSprite() const override{ return image; }
    bool IsActive() const { return active; }


private:
    Float2 pos;          // Current position (shifts during physics updates / animations)
    Float2 originalPos;  // Stored base position to lock back to after a bounce animation
    bool active;
    Image image;

    // Animation / State variables
    bool isBouncing;
    float bounceTimer;
    /*int texActive;
    int texEmpty;*/
};
