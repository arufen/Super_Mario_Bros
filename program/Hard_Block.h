#pragma once
#include "IBlock.h"

class HardBlock : public IBlock
{
public:
    HardBlock();

    void Init(Float2 startPos, int graphHandle);
    void Update() override;
    void Render(Float2 cameraPos) override;

    // Triggered when Mario hits the block from underneath
    void Bump() override;

    Float2 GetPos() const override { return pos; }
    Image GetSprite() const override { return image; }
    bool IsActive() const { return active; }

private:
    Float2 pos;
    bool active;
    Image image;
};
