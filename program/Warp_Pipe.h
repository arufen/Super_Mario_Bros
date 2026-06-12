#pragma once
#include "IBlock.h"
#include "Camera.h"

class Pipe : public IBlock, public Collidable
{
public:
    Pipe();

    void Init(Float2 startPos, int tLeft, int tRight, int bLeft, int bRight, int HeightsInBlock, bool warpEnable);
    void Update() override;
    void Render(Camera camera) override;

    // Triggered when Mario hits the block from underneath
    void OnHitTop(RigidBody& player) override;

    Float2 GetPos() const override { return pos; }
    Image GetSprite() const override { return image; }
    bool IsActive() const { return active; }

private:
    Float2 pos;
    bool active;
    bool isWarpPipe;
    int total_blocks;

    int texTL, texTR, texBL, texBR;// texture handle for 4 parts of pipe

    Image image;
};

class UnderWorldPipe : public IBlock, public Collidable
{
public:
    UnderWorldPipe();

    void Init(Float2 startPos, int graphHandle);
    void Update() override;
    void Render(Camera camera) override;

    // Triggered when Mario hits the block from underneath
    void OnHitSide(RigidBody& player) override;

    Float2 GetPos() const override { return pos; }
    Image GetSprite() const override { return image; }
    bool IsActive() const { return active; }

private:
    Float2 pos;
    bool active;
    //bool isWarpPipe;

    int texPipe;

    Image image;
};
