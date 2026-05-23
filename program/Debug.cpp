#include "Main.h"
#include "Game.h"
#include "Debug.h"
#include "Camera.h"
#include "Mario.h"

// メインのカメラはMain.cppで定義されているため、externを使って参照する
extern Camera MainCamera;

// 現在の表示モードを保持する変数
int map_mode;

//---------------------------------------------------------------------------------
//	初期化処理
//---------------------------------------------------------------------------------
void Debug::Init()
{
		map_mode = MODE_USUALLY;
}

//---------------------------------------------------------------------------------
//	更新処理
//---------------------------------------------------------------------------------
void Debug::Update()
{
	// [Ctrl] + [U] キーで通常モードに切り替える
	if (CheckHitKey(KEY_INPUT_LCONTROL) && PushHitKey(KEY_INPUT_U))
	{
		map_mode = MODE_USUALLY;
	}

	// [Ctrl] + [D] キーでデバッグモードに切り替える
	if( CheckHitKey( KEY_INPUT_LCONTROL ) && PushHitKey( KEY_INPUT_D ) )
	{
		map_mode = MODE_DEBUG;
	}

	// 各モードに応じた追加の更新処理をここに記述（必要に応じて）
	if (map_mode == MODE_USUALLY)
	{

	}

	if (map_mode == MODE_DEBUG)
	{

	}
}

//---------------------------------------------------------------------------------
//	描画処理
//---------------------------------------------------------------------------------
void Debug::Render()
{
	// カメラの現在位置を表示
	DrawFormatString(0, 0, GetColor(255, 255, 255), "camera pos X : %f, camera pos Y : %f", MainCamera.pos.x, MainCamera.pos.y);

	// 現在のモードと、モードに応じた補助線の描画
	if (map_mode == MODE_DEBUG) 
	{
		DrawString(0, 20, "DEBUG MODE ACTIVE", GetColor(255, 0, 0));
		// デバッグモード時は画面中央を示す縦線を引く
		DrawLine(SCREEN_W/2, 0, SCREEN_W/2, SCREEN_H, GetColor(0, 255, 255));

		// マリオの当たり判定矩形（Mario.cpp で更新されるスクリーン座標）を描画
		DrawBox(mario_debug_x1, mario_debug_y1, mario_debug_x2, mario_debug_y2, GetColor(0, 255, 0), FALSE);
		// マリオの画像中央に縦線を引く（mario_centerX はスクリーン座標）
		DrawLine(mario_centerX, mario_debug_y1, mario_centerX, mario_debug_y2, GetColor(0, 255, 0));

		// マリオ左上に黄色の点を表示し、点の横に座標を描画
		DrawFormatString(mario_debug_x1 - 80, mario_debug_y1 - 20, GetColor(255, 255, 255), "(%d,%d)", mario_debug_x1, mario_debug_y1);
		DrawCircle(mario_debug_x1, mario_debug_y1, 5, GetColor(255, 255, 0), TRUE);

		// マリオが歩き状態とダッシュ状態の表示
	}
	else 
	{
		DrawString(0, 20, "USUALLY MODE ACTIVE", GetColor(255, 255, 255));
	}
}
