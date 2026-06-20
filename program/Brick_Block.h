#pragma once
#include "IBlock.h"
#include "Camera.h"
#include "Mario.h"
#include "HelpfulFunc.h"
#include "Brick_BlockParts.h"

// Unified enum to handle all item states inside a brick
enum class BrickBlockItem
{
    NONE, // Regular destructible brick block
    COIN,
    STAR,
};

class BrickBlock : public IBlock, public Collidable
{
public:
    BrickBlock();

    // Standard constructor for normal destructible bricks
    void Init(Float2 startPos, int graphHandle);

    // Multi-purpose constructor for item-bearing bricks (e.g. multi-coin bricks)
    void Init(Float2 startPos, int active_graph, int empty_graph, BrickBlockItem item, int max_hits = 1);

    void Update() override;
    void Render(Camera camera) override;

    // Triggered when Mario hits the block from underneath
    virtual void OnHitBottom(RigidBody& player) override;

    Float2 GetPos() const override { return pos; }
    Image GetSprite() const override { return image; }
    bool IsActive() const { return active; }
    bool IsAvailable() const { return isAvailable; }

    BrickBlockItem itemType;

    // Visual animation structures for a popping coin item
    Animation itemCoinAnim;
    Timer itemCoinAnimationTimer{ 0.5f };

private:
    Float2 pos;          // Dynamic tracking position (bounces during hit sequences)
    Float2 originalPos;  // Stored coordinate layout to lock back to after a bounce
    bool active;
    Image image;

    // State parameters
    bool isBouncing;
    float bounceTimer;
    bool isAvailable;   // True if the brick can still be hit for items

    int hit_count;
    int max_hits_allowed;
    int texActive;
    int texEmpty;
};

