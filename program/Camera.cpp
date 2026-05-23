#include "Main.h"
#include "Camera.h"
#include "Mario.h"
#include "Game.h" // SCREEN_W を参照するためにインクルード

extern Camera MainCamera;

// コンストラクタ
Camera::Camera() 
{
	pos = { 0.0f, 0.0f };
}

// カメラの位置を設定する
void Camera::SetPos(Float2 targetPos)
{
	pos.Set(targetPos);
}

// 画像を描画する際、カメラ位置に応じたスクリーン座標に変換して描画する
void Camera::GlobalRenderImage(const Image& image, float scale)
{
	Float2 globalPos;
	globalPos.Set(image.pos.x - pos.x, image.pos.y - pos.y);
	image.Render(globalPos, scale);
}

void Camera::Update()
{
	// カメラの移動可能範囲の制限（左端：0、右端：12480）
	if( MainCamera.pos.x <= 0){
		MainCamera.pos.x = 0;
	}
	if (MainCamera.pos.x >= 12480) {
		MainCamera.pos.x = 12480;
	}
}