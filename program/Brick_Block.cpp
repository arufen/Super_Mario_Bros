#include "Brick_Block.h"
#include "Dxlib.h"

// Constructor
BrickBlock::BrickBlock()
{
    pos.Set(0.0f, 0.0f);
    originalPos.Set(0.0f, 0.0f);
    active = false;
    isBouncing = false;
    bounceTimer = 0.0f;
}

void BrickBlock::Init(Float2 startPos, int graphHandle)
{
    pos = startPos;
    originalPos = startPos;
    active = true;

    image.InitialImageAndSize(graphHandle);
}

void BrickBlock::Bump()
{
    if (!isBouncing)
    {
        isBouncing = true;
        bounceTimer = 0.0f; // Reset our animation timeframe tracker
        
    }
}

void BrickBlock::Update()
{
    if (!active) return;

    // マリオはスーパーマリオとかファイアマリオになったら…
}

void BrickBlock::Render(Float2 cameraPos)
{
    if (!active) return;

    // Calculate where the block should draw relative to the camera screen space
    Float2 screenPos = { pos.x - cameraPos.x,pos.y - cameraPos.y };

    // Render at the screen relative position
    image.Render(screenPos);
}