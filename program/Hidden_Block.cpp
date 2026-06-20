#include "Hidden_Block.h"
#include "Mario.h"
#include "1-Up_Mushroom.h"
#include "StageManager.h"

HiddenBlock::HiddenBlock()
{
    pos.Set(0.0f, 0.0f);
    isRevealed = false;
    active = false;
    itemSpawned = false;
    itemType = HiddenItemType::MUSHROOM_1UP;
    itemCoinAnim.InitialAnimation(LoadGraph("data/image/item_coin.png"), 4, 10);
}

void HiddenBlock::Init(Float2 startPos, int activeBlockTextureHandle, HiddenItemType type)
{
    pos = startPos;
    texActive = activeBlockTextureHandle;
    isRevealed = false;
    itemSpawned = false;
    active = true;
    itemType = type;

    image.InitialImageAndSize(texActive);
    image.pos = pos;

    // HIDE THE COLLIDER
    collider = Collider(pos.x, -9999.0f, (float)BLOCK_SIZE, (float)BLOCK_SIZE);
}

void HiddenBlock::Update()
{
    if (itemCoinAnimationTimer.GetCurrentTimer() > 0.0f)
    {
        itemCoinAnimationTimer.Update();
        itemCoinAnim.AnimationUpdateLoop();

        float totalTime = itemCoinAnimationTimer.MAX_TIMER;
        float elapsed = totalTime - itemCoinAnimationTimer.GetCurrentTimer();
        float t = elapsed / totalTime;

        float offset = -16.0f;
        float length = 64.0f * 3.5f;

        itemCoinAnim.x = pos.x + image.sizeX / 2.0f - itemCoinAnim.sprite.sizeX / 2.0f;
        itemCoinAnim.y = pos.y + offset - sinf(t * 3.14159f) * length;
    }
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

                OnHitBottom(MainMario);
            }
        }
    }
}

void HiddenBlock::OnHitTop(RigidBody& player) { PushPlayerOut(player); }
//void HiddenBlock::OnHitSide(RigidBody& player) { PushPlayerOut(player); }
void HiddenBlock::OnHitBottom(RigidBody& player)
{
    /*if (!itemSpawned)
    {
        itemSpawned = true;

        SuperMushroom* newMushroom = new SuperMushroom();

        newMushroom->Init(pos.x, pos.y);

        newMushroom->image.InitialImageAndSize(LoadGraph("data/image/HpUpMushroom.png"));

        newMushroom->SpawnItem();


        StageManager::GetInstance().superMushroom.push_back(newMushroom);

        RigidBody::collidables.push_back(newMushroom);
    }*/
    if (!itemSpawned)
    {
        itemSpawned = true;
        if (itemType == HiddenItemType::MUSHROOM_1UP)
        {
            // Spawn the 1up HP Up Mushroom
            UpMushroom* newMushroom = new UpMushroom();
            newMushroom->Init(pos.x, pos.y);
            newMushroom->image.InitialImageAndSize(LoadGraph("data/image/HpUpMushroom.png"));
            newMushroom->SpawnItem();

            StageManager::GetInstance().upMushroom.push_back(newMushroom);
            RigidBody::collidables.push_back(newMushroom);
        }
        else if (itemType == HiddenItemType::COIN)
        {
            itemCoinAnimationTimer.SetTimer(0.4f);
        }
    }
}

void HiddenBlock::Render(Camera camera)
{
    if (itemCoinAnimationTimer.GetCurrentTimer() > 0.0f)
    {
        camera.GlobalRenderAnimation(itemCoinAnim);
    }
    if (!active) return;

    if (isRevealed)
    {
        /*float screenX = pos.x - camera.pos.x;
        float screenY = pos.y - camera.pos.y;
        DrawGraphF(screenX, screenY, texEmpty, TRUE);*/
        camera.GlobalRenderImage(image);
    }
}
void HiddenBlock::PushPlayerOut(RigidBody& player)
{

}