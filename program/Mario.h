#pragma once
#include "HelpfulFunc.h"
#include "Collider.h"
#include "Collidable.h"
#include <vector>
#include "RigidBody.h"
#include "Enemy.h"
#include "Coin.h"
#include "Question_Block.h"
#include "Brick_Block.h"
//#include "Koopa_Troopa.h"
using namespace std;

// 外部ファイル（Camera.cppなど）からマリオの移動速度や中心座標を
// 変更せずに参照できるよう、グローバル変数として公開しています
extern float marioSpeed;
extern int mario_centerX;
// デバッグ描画用にマリオの画面座標の矩形情報を外部参照可能にする
extern int small_mario_debug_x1;
extern int small_mario_debug_y1;
extern int small_mario_debug_x2;
extern int small_mario_debug_y2;
enum class MarioState { NORMAL, WARPING, DEAD };
enum class MarioForm { SMALL, SUPER, FIRE }; // マリオの形態

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

	// ※マリオの初期Y座標が700.0fなので、地上は980.0fに設定しています。地下の高さに合わせて数値は調整してください。
	static constexpr float DEAD_LINE_OVERWORLD = 980.0f;
	static constexpr float DEAD_LINE_UNDERWORLD = 1940.0f;

	static constexpr float WARP_DURATION = 1.0f; // warp timer


	//Mario image/animation variables (画像/アニメーション変数)
	Image small_mario_waitImage;	 // マリオの待機画像情報
	Image small_mario_jumpImage;   // マリオのジャンプ画像情報
	Image small_mario_deadImage;   // マリオの死亡画像情報
	Animation small_mario_walkAnim;		 // 歩きアニメーション管理オブジェクト

	// 後入力優先のためのキー状態保持
	bool prevKeyA = false;
	bool prevKeyD = false;
	bool preferLeftInput = false;
	bool prevKeySpace = false;

	bool isLeft;		// 向きフラグ（trueなら左向き、falseなら右向き）
	bool isWalking = false;     // 現在歩いているかどうか
	bool isJumping = false;
	bool isDeadJumped = false;	// 死亡ジャンプをすでに受け取ったか
	bool isFellDown = false;	// 落下死したかどうか
	float jumpHoldTimer = 0.0f;

	// State machine controllers
	MarioState currentState = MarioState::NORMAL;
	float warpTimer = 0.0f;

	//item and score (アイテムとスコア)
	int score = 0;
	int coin = 0;

	//star power-up variables(スター状態の取得)
	Animation starmarioAnim; 
	bool isStarMode;         
	float starTimer;       
	Timer invincibleTimer{ 0.0f };

	void ResolveCollision(Collidable& block) override;

	// 現在のマリオの形態を取得・変更する関数
	MarioForm GetForm() const { return currentForm; }
	void SetForm(MarioForm form) { currentForm = form; }

	// ゲーム管理クラス（Game.cppなど）からマリオの状態を確認するための関数
	MarioState GetState() const { return currentState; }

	// エネミーに横・下から接触したときに死亡状態へ移行させる関数
	void ToDeadState(bool isFall = false);

	//for main thread
	void Init();
	void Update();
	void Render();

	//mechanics function
	void AddCoin();
	void Jump();


	void Star();
	


	//void Warping(float pipeY);

	enum class WarpDir { DOWN, RIGHT, UP, };
	WarpDir currentWarpDir = WarpDir::DOWN;

	// Update the function signature
	void Warping(float targetX, float targetY, WarpDir dir);

private:
	int texWarp;
	MarioForm currentForm = MarioForm::SMALL; // デフォルトはスモールマリオ
};