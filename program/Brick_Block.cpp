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



    Mario* mario = dynamic_cast<Mario*>(&player);

    // アイテム入り、またはチビマリオの時は壊れない
    if (itemType != BrickBlockItem::NONE || max_hits_allowed > 0 || (mario && mario->GetForm() == MarioForm::SMALL))
    {
        if (isBouncing) return;

        isBouncing = true;
        bounceTimer = 0.0f;
        SoundManager::GetInstance().PlaySE("Bump");

        if (itemType == BrickBlockItem::COIN)
        {
            itemCoinAnimationTimer.ResetTimer();
            MainScore.AddScore(150);
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

        hit_count++;
        if (max_hits_allowed > 0)
        {
            if (hit_count >= max_hits_allowed)
            {
                isAvailable = false;
                itemType = BrickBlockItem::NONE;
                image.InitialImageAndSize(texEmpty);
            }
        }
        return;
    }
    // -------------------------------------------------------------
    // --- 通常のレンガブロック破壊 ＆ 新しい名称での破片生成処理 ---
    //// -------------------------------------------------------------
    SoundManager::GetInstance().PlaySE("Break");
    
    MainScore.AddScore(50);
    // 64x64ブロックの物理的な中心点を計算
    float centerX = pos.x + 32.0f;
    float centerY = pos.y + 32.0f;

    // StageManager から画像ハンドルを取得
    int handle = StageManager::GetInstance().GetBrickPartsHandle();

    // 4つの破片オブジェクトをポインタとして動的生成 (new)
    // ※注: `BrickPiece` の部分は、あなたが `BrickBlockParts` を継承して作った実際の具象クラス名（実体化できるクラス名）に書き換えてください。
    BrickBlockParts* p1 = new BrickBlockParts();
    BrickBlockParts* p2 = new BrickBlockParts();
    BrickBlockParts* p3 = new BrickBlockParts();
    BrickBlockParts* p4 = new BrickBlockParts();

    // それぞれを初期化・スポーンさせる
    // 左上: 左上に高く飛び、反時計回りに回転
    p1->Spawn(Float2(centerX - 16.0f, centerY - 16.0f), Float2(-2.5f, -8.5f), -0.12f, handle);

    // 右上: 右上に高く飛び、時計回りに回転
    p2->Spawn(Float2(centerX + 16.0f, centerY - 16.0f), Float2(2.5f, -8.5f), 0.12f, handle);

    // 左下: 左に低めの軌道で飛び、反時計回りに回転
    p3->Spawn(Float2(centerX - 16.0f, centerY + 16.0f), Float2(-1.8f, -5.5f), -0.08f, handle);

    // 右下: 右に低めの軌道で飛び、時計回りに回転
    p4->Spawn(Float2(centerX + 16.0f, centerY + 16.0f), Float2(1.5f, -5.5f), 0.08f, handle);

    // StageManager のポインタ配列に追加して管理を委ねる
    StageManager::GetInstance().brickParts.push_back(p1);
    StageManager::GetInstance().brickParts.push_back(p2);
    StageManager::GetInstance().brickParts.push_back(p3);
    StageManager::GetInstance().brickParts.push_back(p4);

    // ブロック自体を非アクティブ化し、当たり判定を消失させる
    active = false;
    isAvailable = false;
    collider.width = 0;
    collider.height = 0;
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
