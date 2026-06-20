#pragma once
#include "Collidable.h"
#include "Camera.h"
#include "HelpfulFunc.h"

// 土台部分（固いブロック）用のクラス
class GoalPoleBase : public Collidable
{
public:
	GoalPoleBase() {
		isTrigger = false; // すり抜けない（固いブロック）
	}

	// ぶつかった時の特別な処理は不要なので空にしておく
	virtual void OnHitTop(RigidBody& player) override {}
	virtual void OnHitBottom(RigidBody& player) override {}
	virtual void OnHitSide(RigidBody& player) override {}
};

class GoalPole : public Collidable
{
public:
	Image image;
	Image flagImage;// 旗の画像

	bool isReached; // マリオが触れたかどうかのフラグ
	float maxFlagY; // 旗が下りる限界のY座標

	// クリアBGM制御用の変数
	bool isBGMPlayed; // すでにクリアBGMを流したか
	float clearTimer;  // 降りきってからの時間を計るタイマー

	// 土台用の物理判定
	GoalPoleBase baseCollider;

	GoalPole();

	void Init(float x, float y);
	void Update();
	void RenderGlobal(Camera& camera);

	virtual void OnHitSide(RigidBody& player);
	virtual void OnHitTop(RigidBody& player);
	virtual void OnHitBottom(RigidBody& player);
};