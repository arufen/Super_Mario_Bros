#include "Main.h"
#include "Game.h"
#include "Map.h"
#include "Debug.h" // map_modeなどのモード定数を参照するためにインクルード

void Map::Init()
{
    // 通常モード用のマップ画像を読み込み、基準位置を設定する
    mapImageUsually.InitialImageAndSize(LoadGraph("data/1-1.png"));
    mapImageUsually.pos.Set(0.0f, 0.0f);

    // デバッグモード用（当たり判定可視化など）のマップ画像を読み込み、基準位置を設定する
    mapImageDebug.InitialImageAndSize(LoadGraph("data/demo_1-1.png"));
    mapImageDebug.pos.Set(0.0f, 0.0f);
}

void Map::Render(Camera& camera)
{
    // 現在の表示モードを判定して、描画する画像を切り替える
    if (map_mode == MODE_USUALLY)
    {
        // 4倍スケールで通常マップを描画
        camera.GlobalRenderImage(mapImageUsually, 4.0f);
    }
    else if (map_mode == MODE_DEBUG)
    {
        // 4倍スケールでデバッグ用マップを描画
        camera.GlobalRenderImage(mapImageDebug, 4.0f);
    }
}