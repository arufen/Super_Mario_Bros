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
    image.pos = pos;
    collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);
}

void HardBlock::Update()
{
    //if (!active) return;
}

void HardBlock::Render(Camera camera)
{
    if (!active) return;

    camera.GlobalRenderImage(image);
}