#pragma once
#include "HelpfulFunc.h"
#include "Camera.h"

// マップの描画や管理を行うクラス
// 現状は一枚絵ですが、将来的にマップチップなどによる管理へ拡張できる構造にしています
class Map
{
private:
    Image mapImageUsually; // 通常表示用のマップ画像
    Image mapImageDebug;   // デバッグ表示用のマップ画像

public:
    void Init();
    
    // 描画処理 (カメラの座標や設定を適用して描画するため、Cameraの参照を受け取る)
    void Render(Camera& camera);
};