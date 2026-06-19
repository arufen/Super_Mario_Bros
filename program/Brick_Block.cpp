#include "Brick_Block.h"
#include "Dxlib.h"
#include "Mario.h"
#include "Sound.h"
#include <cmath>
#include "StageManager.h"

BrickBlock::BrickBlock()
{
    pos.Set(0.0f, 0.0f);
    originalPos.Set(0.0f, 0.0f);
    active = false;
    isBouncing = false;
    isAvailable = true;
    bounceTimer = 0.0f;
    hit_count = 0;
    max_hits_allowed = 1;
    itemType = BrickBlockItem::NONE;
    itemCoinAnim.InitialAnimation(LoadGraph("data/image/item_coin.png"), 4, 10);
}

// Initialization for regular breakable bricks
void BrickBlock::Init(Float2 startPos, int graphHandle)
{
    pos = startPos;
    originalPos = startPos;
    active = true;
    isAvailable = true;
    hit_count = 0;
    max_hits_allowed = 0; // 0 hits means it is breakable instead of turning brown
    itemType = BrickBlockItem::NONE;
    texActive = graphHandle;
    texEmpty = graphHandle;

    image.InitialImageAndSize(graphHandle);
    image.pos = pos;
    collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);
}

// Initialization for item bricks
void BrickBlock::Init(Float2 startPos, int active_graph, int empty_graph, BrickBlockItem item, int max_hits)
{
    pos = startPos;
    originalPos = startPos;
    active = true;
    isAvailable = true;
    hit_count = 0;
    max_hits_allowed = max_hits;
    itemType = item;
    texActive = active_graph;
    texEmpty = empty_graph;

    image.InitialImageAndSize(texActive);
    image.pos = pos;
    collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);

    if (itemType == BrickBlockItem::COIN)
    {
        //itemCoinAnim.InitialAnimation(LoadGraph("data/image/item_coin.png"), 4, 10);
        itemCoinAnim.x = pos.x + image.sizeX / 2.0f - itemCoinAnim.sprite.sizeX / 2.0f;
        itemCoinAnim.y = pos.y + image.sizeY / 2.0f - itemCoinAnim.sprite.sizeY / 2.0f;
    }
}


void BrickBlock::OnHitBottom(RigidBody& player)
{
    if (!isAvailable) return;

    // Start block bounce sequence
    isBouncing = true;
    bounceTimer = 0.0f;

    Mario* mario = dynamic_cast<Mario*>(&player);

    if (itemType == BrickBlockItem::NONE)
    {
        // Check Mario form using Mario.h state tracking rules
        if (mario && mario->GetForm() != MarioForm::SMALL)
        {
            // Big or Fire Mario breaks the block entirely
            active = false;
            isAvailable = false;

            SoundManager::GetInstance().PlaySE("Break");
            // TODO: Play brick shatter sound effect
        }
        else
        {
            // Small Mario makes the block bounce, but it remains unbroken
            SoundManager::GetInstance().PlaySE("Bump");
        }
    }
    else
    {
        // Item handling behavior
        hit_count++;

        if (itemType == BrickBlockItem::COIN)
        {
            itemCoinAnimationTimer.ResetTimer();
            if (mario)
            {
                mario->coin++; // Increment Mario's coin total safely
            }
        }
        else if (itemType == BrickBlockItem::STAR)
        {
            // 1. Create a new instance of SuperStar
            SuperStar* newStar = new SuperStar();

            // 2. Initialize it just slightly above or matching the block's layout position
            newStar->Init(pos.x, pos.y);

            // 3. Trigger its rising spawning state sequence
            newStar->SpawnItem();

            // 4. Push it into StageManager's star tracker vector so it updates and renders globally
            StageManager::GetInstance().superStar.push_back(newStar);
            RigidBody::collidables.push_back(newStar);
        }

        // Lock item production once hit maximum is reached
        if (hit_count >= max_hits_allowed)
        {
            isAvailable = false;
            image.InitialImageAndSize(texEmpty); // Swaps texture permanently to empty brown block
        }
    }
}

void BrickBlock::Update()
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

    if (!active) return;

    // Fast arcade block bounce calculation
    if (isBouncing)
    {
        bounceTimer += 1.0f;

        if (bounceTimer <= 6.0f)
        {
            pos.y -= 3.0f;
        }
        else if (bounceTimer <= 12.0f)
        {
            pos.y += 3.0f;
        }
        else
        {
            pos.y = originalPos.y;
            isBouncing = false;
        }
    }
}

void BrickBlock::Render(Camera camera)
{
    // Draw item coin burst layer if active
    if (itemCoinAnimationTimer.GetCurrentTimer() > 0.0f)
    {
        camera.GlobalRenderAnimation(itemCoinAnim);
    }
    if (!active) return;

    // Render using standard non-rotated method
    image.pos = pos;
    camera.GlobalRenderImage(image);
}