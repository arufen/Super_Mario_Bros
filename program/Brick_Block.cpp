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
    
    isBouncing = false;
    bounceTimer = 0.0f;

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