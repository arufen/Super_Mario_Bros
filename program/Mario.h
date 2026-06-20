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
#include "CollectableItem.h"
#include "Hidden_Block.h"

//Exclude macro from winspool.h
#undef GetForm
#undef SetForm

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
enum class MarioState { NORMAL, WARPING, CUTSCENE, DEAD, GOAL};
enum class MarioForm { SMALL, SUPER, FIRE }; // マリオの形態

//==========================================================================================================
// スター状態の管理クラス(Marioクラスでもスター状態を使用したいのでここに追記させてもらいます。)※firemarioの実装も同様に行う予定です。
// スター状態のアニメーションやタイマーを管理するクラスを作成して、Mario クラスから切り離すこともできます。
//==========================================================================================================
class StarEffect
{
public:
	bool isActive;
	float timer;

	// 状態ごとの点滅アニメーションを個別に持つ
	Animation waitAnim;
	Animation walkAnim;
	Animation jumpAnim;

	Animation bigWaitAnim;
	Animation bigWalkAnim;
	Animation bigJumpAnim;

	void Init();
	void Start(); // スター状態開始 (starTimer = 600.0f など)
	// 歩き状態、ジャンプ状態、歩きの速度(FPS)を受け取って更新する
	void Update(bool isWalking, bool isJumping, int walkFPS);
	// 状態を受け取って適切なアニメーションを描画する
	void Render(int screenX, int screenY, bool isLeft, bool isJumping, bool isWalking, MarioForm form);
};
//==========================================================================================================

// プレイヤーキャラクター（マリオ）の挙動や描画を管理するクラス
class Mario : public RigidBody
{
public:

	// 地上での挙動
	static constexpr float MARIO_ACCEL = 0.12f;          // 1フレームごとの加速（初代はジワッと加速する）
	static constexpr float MARIO_WALK_MAX_SPEED = 4.0f;   // 通常歩行の最高速度
	static constexpr float MARIO_DASH_MAX_SPEED = 7.5f;   // Bダッシュ時の最高速度
	static constexpr float MARIO_FRICTION = 0.35f;        // キーを離した時の摩擦（これによって少し滑って止まる）
	static constexpr float MARIO_DECEL_TURN = 0.35f;      // 地上で逆キーを入れたときの急ブレーキの強さ

	// 空中での慣性挙動（地上よりコントロールを鈍くする）
	static constexpr float MARIO_AIR_ACCEL = 0.4f;       // 空中での横加速
	static constexpr float MARIO_AIR_FRICTION = 0.02f;    // 空中での自然減速（ほぼ滑る）

	// ジャンプ制御
	static constexpr float JUMP_HOLD_MAX = 0.4f;         // ボタン長押しで高く飛べる最大時間
	static constexpr float JUMP_INITIAL_WALK = -15.5f;    // 通常・歩きジャンプの初速
	static constexpr float JUMP_INITIAL_DASH = -17.0f;    // ダッシュジャンプの初速（Bダッシュ大ジャンプ！）
	static constexpr float JUMP_HOLD_FORCE = -0.6f;      // 長押し中の追加上昇力

	// ※マリオの初期Y座標が700.0fなので、地上は980.0fに設定しています。地下の高さに合わせて数値は調整してください。
	static constexpr float DEAD_LINE_OVERWORLD = 980.0f;
	static constexpr float DEAD_LINE_UNDERWORLD = 1940.0f;

	static constexpr float WARP_DURATION = 1.0f; // warp timer

	static constexpr float COLLIDER_OFFSET = 20.0f; //shirnk collider offset


	//Mario image/animation variables (画像/アニメーション変数)
	Image small_mario_waitImage;		// マリオの待機画像情報
	Image small_mario_jumpImage;		// マリオのジャンプ画像情報
	Image small_mario_deadImage;		// マリオの死亡画像情報
	Image small_mario_goalpoleImage;	// ゴールポール用の画像情報
	Animation small_mario_walkAnim;		// 歩きアニメーション管理オブジェクト
	Image big_mario_waitImage;	// でかい状態のマリオの待機画像
	Image big_mario_fire_waitImage;
	Image big_mario_jumpImage;	// でかい状態のマリオのジャンプ画像
	Animation bigmario_moveAnim;//でかい状態のマリオの歩きアニメーション
	Image big_mario_turnImage;	// でかい状態のマリオの方向転換画像




	// 後入力優先のためのキー状態保持
	bool prevKeyA = false;
	bool prevKeyD = false;
	bool preferLeftInput = false;
	bool prevKeySpace = false;

	bool isLeft;		// 向きフラグ（trueなら左向き、falseなら右向き）
	bool isWalking = false;     // 現在歩いているかどうか
	bool isJumping = false;
	bool isBraking = false;		// ブレーキ（急停止）状態かどうか
	bool isDeadJumped = false;	// 死亡ジャンプをすでに受け取ったか
	bool isFellDown = false;	// 落下死したかどうか
	float jumpHoldTimer = 0.0f;

	// State machine controllers
	MarioState currentState = MarioState::NORMAL;
	float warpTimer = 0.0f;

	//item and score (アイテムとスコア)
	int score = 0;
	int coin = 0;

	//スター状態の管理オブジェクト
	StarEffect starEffect;


	Timer invincibleTimer{ 0.0f };

	void ResolveCollision(Collidable& block) override;

	// 現在のマリオの形態を取得・変更する関数
	MarioForm GetForm() const { return currentForm; }
	void SetForm(MarioForm form);

	// ゲーム管理クラス（Game.cppなど）からマリオの状態を確認するための関数
	MarioState GetState() const { return currentState; }


	void TakeDamage();
	// エネミーに横・下から接触したときに死亡状態へ移行させる関数
	void ToDeadState(bool isFall = false);


	//for main thread
	void Init();
	void Update();
	void Render();

	//mechanics function
	void AddCoin();
	void Jump(bool playSound = true);
	
	//For cutscene
	void StartCutsceneWalk(float targetX, float speed);
	void WalkTo(float targetX, float speed);
	void StartGoalCutscene(float poleX, float castleX, float floorY);
	float cutsceneTargetX = 0.0f;
	float cutsceneSpeed = 2.0f;

	// ゴール演出用の管理変数
	int goalPhase = 0;         // 0:降りる, 1:反転, 2:歩く, 3:消える
	float goalPoleX = 0.0f;    // ポールのX座標
	float goalCastleX = 0.0f;  // 城の入り口のX座標
	float goalFloorY = 0.0f;   // 着地する床（土台）のY座標

	//void Warping(float pipeY);

	enum class WarpDir { DOWN, RIGHT, UP, };
	WarpDir currentWarpDir = WarpDir::DOWN;

	// Update the function signature
	void Warping(float targetX, float targetY, WarpDir dir);

	//For debug (DEBUG用)
	bool debug_is_inivincible;

private:
	int texWarp;
	MarioForm currentForm = MarioForm::SMALL; // デフォルトはスモールマリオ
};