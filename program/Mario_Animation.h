//========================================================
//! マリオのアニメーション管理クラス
//======================================================
#pragma once
#ifndef __MARIO_ANIMATION_H__
#define __MARIO_ANIMATION_H__

#include "HelpfulFunc.h"

class MarioAnimation
{
public:
	Animation walkAnimation;

	// 初期化
	void Init();

	// アニメーションの更新
	void Update();

	// 描画
	void Render();
};

#endif // __MARIO_ANIMATION_H__