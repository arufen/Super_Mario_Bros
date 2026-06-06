#pragma once
#include "HelpfulFunc.h"
#include "Collider.h"
#include "Collidable.h"
#include <vector>
#include "RigidBody.h"
using namespace std;

// 外部ファイル（Camera.cppなど）からマリオの移動速度や中心座標を
// 変更せずに参照できるよう、グローバル変数として公開しています
extern float marioSpeed;
extern int mario_centerX;
// デバッグ描画用にマリオの画面座標の矩形情報を外部参照可能にする
extern int mario_debug_x1;
extern int mario_debug_y1;
extern int mario_debug_x2;
extern int mario_debug_y2;
enum class MarioState{NORMAL, WARPING};

// プレイヤーキャラクター（マリオ）の挙動や描画を管理するクラス
class Mario : public RigidBody
{
public:

	//const
	static constexpr float GRAVITY = 1.0f;	// gravity (重力)
	static constexpr float JUMP_FORCE = 1.0f;	// jump force (ジャンプの初速)
	static constexpr float MARIO_ACCEL = 0.4f;		// 1フレームごとの加速度（増やすとキレが良くなる）
	static constexpr float MARIO_WALK_MAX_SPEED = 4.5f;	// 歩き状態の最高速度（これ以上速くならない）
	static constexpr float MARIO_DASH_MAX_SPEED = 8.0f;   // ダッシュ状態の最高速度（これ以上速くならない）
	static constexpr float MARIO_FRICTION = 0.3f;	// キーを離したときの摩擦・ブレーキ（減らすとよく滑る）
	static constexpr float MARIO_DECEL_TURN = 0.8f;  // 逆キーを入れたときの急ブレーキの強さ

	static constexpr float JUMP_HOLD_MAX = 0.3f;   // max seconds u can hold for extra boost
	static constexpr float JUMP_INITIAL = -15;  // first jump force (negative = up)
	static constexpr float JUMP_HOLD_FORCE = -0.7f; // extra boost per frame while holding

	static constexpr float WARP_DURATION = 1.0f; // warp timer


	//Mario image/animation variables (画像/アニメーション変数)
	Image marioImage;	// マリオの画像情報（サイズなどを含む）

	//Mario game mechanics variables (メカニック変数)
	//Float2 position; //Mario position (座標)
	//float now_speed_x;	// 現在の横方向の速度
	//float now_speed_y;	// 現在の縦方向の速度
	bool isLeft;		// 向きフラグ（trueなら左向き、falseなら右向き）
	bool isJumping = false;
	float jumpHoldTimer = 0.0f;

	// State machine controllers
	MarioState currentState = MarioState::NORMAL;
	float warpTimer = 0.0f;

	//Collider collider; // Collider (マリオの当たり判定)

	////Check Every collision that has registered here （登録した当たり判定をチェックする）
	//vector<Collidable*> collidables; // all blocks register here （登録の当たり判定）

	void ResolveCollision(Collidable& block) override;


	//for main thread
	void Init();
	void Update();
	void Render();

	//mechanics function
	void Jump();

	void Warping(float pipeY);

private:
	int texWarp;
};