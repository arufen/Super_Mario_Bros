#pragma once
#include "IBlock.h"

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
