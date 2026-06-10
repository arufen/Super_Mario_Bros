#pragma once
#include "IBlock.h"
#include "HelpfulFunc.h"

enum class QuestionBlockItem 
{ 
    COIN, 
    SUPER_MUSHROOM, 
    STARMAN,
	ONE_UP_MUSHROOM,
};

class QuestionBlock : public IBlock, public Collidable
{
public:
    QuestionBlock();

    void Init(Float2 startPos, int active_graph, int empty_graph);
    void Update() override;
    void Render(Camera camera) override;

    // Triggered when Mario hits the block from underneath
    virtual void OnHitBottom(RigidBody& player) override;

    Float2 GetPos() const override { return pos; }
    Image GetSprite() const override { return image; }
    bool IsActive() const { return active; }
	bool IsAvailable() const { return isAvailable; }

	//Types of items that can be spawned from the question block
    QuestionBlockItem itemType;

	Animation itemCoinAnim; // Coin animation that plays when the block is hit
    Timer itemCoinAnimationTimer{0.5f}; // Timer to control the duration of the coin animation

private:
    Float2 pos;          // Current position (shifts during physics updates / animations)
    Float2 originalPos;  // Stored base position to lock back to after a bounce animation
    bool active;
    Image image;

    // Animation / State variables
    bool isBouncing;
    bool isAvailable;// ƒAƒCƒeƒ€‚ª‚ ‚é‚È‚çTrue
    float bounceTimer;

    int texActive;
    int texEmpty;
    
};
