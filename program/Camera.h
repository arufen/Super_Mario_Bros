#pragma once
#include "Float2.h"
#include "HelpfulFunc.h"

class Camera
{
public: 
	Float2 pos;

	// コンストラクタ
	// ※posはカメラの中心ではなく、画面左上角に相当するワールド座標を表します
	Camera();

	void SetPos(Float2 targetPos);

	// カメラ座標を基準に画像を描画する処理（引数は参照渡しで無駄なコピーを防止）
    void GlobalRenderImage(const Image& image, float scale = 1.0f);

	// カメラの更新処理（移動範囲の制限など）
	void Update();
};