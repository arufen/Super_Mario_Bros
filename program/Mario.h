#pragma once
#include "HelpfulFunc.h"

// 外部ファイル（Camera.cppなど）からマリオの移動速度や中心座標を
// 変更せずに参照できるよう、グローバル変数として公開しています
extern float marioSpeed;
extern int mario_centerX;
// デバッグ描画用にマリオの画面座標の矩形情報を外部参照可能にする
extern int mario_debug_x1;
extern int mario_debug_y1;
extern int mario_debug_x2;
extern int mario_debug_y2;

// プレイヤーキャラクター（マリオ）の挙動や描画を管理するクラス
class Mario
{
private:
	Image marioImage;	// マリオの画像情報（座標、サイズなどを含む）
	float now_speed_x;	// 現在の横方向の速度
	bool isLeft;		// 向きフラグ（trueなら左向き、falseなら右向き）

public:
	void Init();
	void Update();
	void Render();
};