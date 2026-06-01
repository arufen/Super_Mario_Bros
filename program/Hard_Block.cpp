#include "Hard_Block.h"
#include "DxLib.h"

HardBlock::HardBlock()
{
    pos.Set(0.0f, 0.0f);
    active = false;
}

void HardBlock::Init(Float2 startPos, int graphHandle)
{
    pos = startPos;
    active = true;

    image.InitialImageAndSize(graphHandle);
}

void HardBlock::Bump()
{
    
}

void HardBlock::Update()
{
    if (!active) return;
}

void HardBlock::Render(Float2 cameraPos)
{
    if (!active) return;

    // Calculate where the block should draw relative to the camera screen space
    Float2 screenPos = { pos.x - cameraPos.x,pos.y - cameraPos.y };

    // Render at the screen relative position
    image.Render(screenPos);
}