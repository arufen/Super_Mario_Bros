#include "Hidden_Block.h"
#include "Mario.h"

HiddenBlock::HiddenBlock(Float2 startPos, int emptyBlockTextureHandle)
{
    pos = startPos;
    texEmpty = emptyBlockTextureHandle;
    isRevealed = false;
    active = true;

    // HIDE THE COLLIDER
    collider = Collider(pos.x, -9999.0f, (float)BLOCK_SIZE, (float)BLOCK_SIZE);
}

void HiddenBlock::Update()
{
    if (!isRevealed)
    {
        extern Mario MainMario;

        // Manual overlap check: Is Mario jumping up through our real secret tile?
        float mLeft = MainMario.position.x;
        float mRight = MainMario.position.x + 32;
        float mTop = MainMario.position.y;

        // Check if Mario's horizontal bounding box crosses our block position
        bool crossX = (mRight > pos.x) && (mLeft < pos.x + BLOCK_SIZE);

        // Check if Mario's head crosses the bottom edge while moving UP (now_speed_y is negative)
        if (crossX && MainMario.now_speed_y < 0.0f)
        {
            if (mTop <= pos.y + BLOCK_SIZE && mTop >= pos.y)
            {
                isRevealed = true;
                MainMario.now_speed_y = 0.0f;

                collider.y = pos.y;
            }
        }
    }
}

void HiddenBlock::OnHitTop(RigidBody& player) { PushPlayerOut(player); }
//void HiddenBlock::OnHitSide(RigidBody& player) { PushPlayerOut(player); }
void HiddenBlock::OnHitBottom(RigidBody& player) { PushPlayerOut(player); }

void HiddenBlock::Render(Camera camera)
{
    if (!active) return;

    if (isRevealed)
    {
        float screenX = pos.x - camera.pos.x;
        float screenY = pos.y - camera.pos.y;
        DrawGraphF(screenX, screenY, texEmpty, TRUE);
    }
}
void HiddenBlock::PushPlayerOut(RigidBody& player)
{

}