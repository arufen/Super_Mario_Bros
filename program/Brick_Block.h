#pragma once
#include "IBlock.h"
#include "Camera.h"

enum class CoinBrickBlockItem
{
    COIN,
};

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
    /*bool isBouncing;
    float bounceTimer;*/
    /*int texActive;
    int texEmpty;*/
};

class CoinBrickBlock : public IBlock, public Collidable
{
public:
    CoinBrickBlock();

    void Init(Float2 startPos, int active_graph, int next_graph);
    void Update() override;
    void Render(Camera camera);

    // Triggered when Mario hits the block from underneath
    virtual void OnHitBottom(RigidBody& player) override;

    Float2 GetPos() const override { return pos; }
    Image GetSprite() const override { return image; }
    bool IsActive() const { return active; }
    bool IsAvailable() const { return isAvailable; }

    //Types of items that can be spawned from the question block
    CoinBrickBlockItem itemType;

    Animation itemCoinAnim; // Coin animation that plays when the block is hit
    Timer itemCoinAnimationTimer{ 0.5f }; // Timer to control the duration of the coin animation


private:
    Float2 pos;          // Current position (shifts during physics updates / animations)
    Float2 originalPos;  // Stored base position to lock back to after a bounce animation
    bool active;
    Image image;

    bool isBouncing;
    bool isAvailable;// ƒAƒCƒeƒ€‚ª‚ ‚é‚È‚çTrue
    float bounceTimer;

    int hit_count;

    int texActive;
    int texNext;
};
