#include "Brick_Block.h"
#include "Dxlib.h"
#include "Mario.h"
#include "Sound.h"

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
    image.pos = pos;
    collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);
}

void BrickBlock::OnHitBottom(RigidBody& player)
{
    // 引数の player を Mario クラスに安全にキャスト
    //ignore object other than mario
    Mario* mario = dynamic_cast<Mario*>(&player);
    if (!mario) return;

    if (mario != nullptr)
    {
        // スモールマリオの時はブロックを壊さない！
        if (mario->GetForm() == MarioForm::SMALL)
        {
            // まだ跳ねていない時だけ跳ね返りアニメーションを開始
            if (!isBouncing)
            {
                isBouncing = true;
                bounceTimer = 0.0f;
                // ここでブロックを叩いた時（壊れない方）のSEを鳴らすとさらに最高です！
                SoundManager::GetInstance().PlaySE("Bump");
            }
            return; // 破壊処理にいかせず、ここで終了する
        }
    }

    // スーパーマリオ以上の場合は、従来通りブロックを破壊（消滅）させる
    active = false;

    collider.x = -9999.0f;
    collider.y = -9999.0f;
    collider.width = 0;
    collider.height = 0;
}
void BrickBlock::Update()
{
    if (!active) return;

    // 叩かれた時の「ポコッ」という跳ね返りアニメーション処理
    if (isBouncing)
    {
        bounceTimer += 1.0f; // 毎フレーム 1.0 ずつ進める

        // 前半6フレーム：高速で上に移動（1フレームにつき3px、計18px上昇）
        if (bounceTimer <= 6.0f)
        {
            pos.y -= 3.0f;
        }
        // 後半6フレーム：同じ速度で下に移動して戻る
        else if (bounceTimer <= 12.0f)
        {
            pos.y += 3.0f;
        }
        // 12フレームを超えたらアニメーション終了
        else
        {
            pos.y = originalPos.y; // ズレを防ぐために元の位置にピッタリ固定
            isBouncing = false;
        }
    }

    // 画像とコライダーの位置を現在の pos に同期（これで浮いている間も判定がズレません）
    image.pos = pos;
    collider.x = pos.x;
    collider.y = pos.y;

    /*if (!active) return;*/



    // マリオはスーパーマリオとかファイアマリオになったら…
}

void BrickBlock::Render(Camera camera)
{
    if (!active) return;

    camera.GlobalRenderImage(image);
}

CoinBrickBlock::CoinBrickBlock()
{
    pos.Set(0.0f, 0.0f);
    originalPos.Set(0.0f, 0.0f);
    active = false;

    isBouncing = false;
    isAvailable = true;
    bounceTimer = 0.0f;
}

void CoinBrickBlock::Init(Float2 startPos, int active_graph, int next_graph)
{
    pos = startPos;
    originalPos = startPos;
    texActive = active_graph;
    texNext = next_graph;
    active = true;
    hit_count = 0;

    image.InitialImageAndSize(texActive);
    image.pos = pos;
    collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);

    itemType = CoinBrickBlockItem::COIN; // Default item type (can be set to other types as needed)

    if (itemType == CoinBrickBlockItem::COIN)
    {
        itemCoinAnim.InitialAnimation(LoadGraph("data/image/item_coin.png"), 4, 10);
        itemCoinAnim.x = pos.x + image.sizeX / 2.0f - itemCoinAnim.sprite.sizeX / 2.0f;
        itemCoinAnim.y = pos.y + image.sizeY / 2.0f - itemCoinAnim.sprite.sizeY / 2.0f;
    }
}

void CoinBrickBlock::OnHitBottom(RigidBody& player)
{
    if (!isAvailable || isBouncing) return;

    hit_count++;

    isBouncing = true;
    bounceTimer = 0.0f;

    itemCoinAnimationTimer.ResetTimer();
}
void CoinBrickBlock::Update()
{
    if (!itemCoinAnimationTimer.TimerHit())
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

    if (isBouncing)
    {
        bounceTimer += 1.0f;

        // Up for 6 frames, down for 6 frames
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
            pos.y = originalPos.y; // Snap back to original position
            isBouncing = false;

            // Only disable the block if it has reached max hits
            if (hit_count >= 9)
            {
                isAvailable = false;
                image.InitialImageAndSize(texNext); // Change texture immediately when it empties
            }
        }
    }

    image.pos = pos;
    collider.y = pos.y;
    collider.x = pos.x;
}


void CoinBrickBlock::Render(Camera camera)
{
    if (!itemCoinAnimationTimer.TimerHit())
    {
        camera.GlobalRenderAnimation(itemCoinAnim);
    }

    if (!active) return;
    camera.GlobalRenderImage(image);
}