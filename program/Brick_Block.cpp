#include "Brick_Block.h"
#include "Dxlib.h"

// Constructor
BrickBlock::BrickBlock()
{
    pos.Set(0.0f, 0.0f);
    originalPos.Set(0.0f, 0.0f);
    active = false;
}

void BrickBlock::Init(Float2 startPos, int graphHandle)
{
    pos = startPos;
    originalPos = startPos;
    active = true;
    

    image.InitialImageAndSize(graphHandle);
    image.pos = pos;
    collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);
}

void BrickBlock::OnHitBottom(RigidBody& player)
{
    //ignore object other than mario
    Mario* mario = dynamic_cast<Mario*>(&player);
    if (!mario) return;

    active = false;

    collider.x = -9999.0f;
    collider.y = -9999.0f;
    collider.width = 0;
    collider.height = 0;
}
void BrickBlock::Update()
{
    /*if (!active) return;*/



    // マリオはスーパーマリオとかファイアマリオになったら…
}

void BrickBlock::Render(Camera camera)
{
    if (!active) return;

    camera.GlobalRenderImage(image);
}