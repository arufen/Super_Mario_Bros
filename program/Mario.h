#pragma once
#include "HelpfulFunc.h"

// 外部ファイル（Camera.cppなど）からマリオの移動速度や中心座標を
// 変更せずに参照できるよう、グローバル変数として公開しています
extern float marioSpeed;
extern int mario_centerX;

// プレイヤーキャラクター（マリオ）の挙動や描画を管理するクラス
class Mario
{
private:
	Image marioImage; // マリオの画像情報（座標、サイズなどを含む）
	bool isLeft;      // 向きフラグ（trueなら左向き、falseなら右向き）

public:
	void Init();
	void Update();
	void Render();
};