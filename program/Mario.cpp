#include "Main.h"
#include "Game.h"
#include "Mario.h"
#include "Debug.h"
#include "Camera.h"
#include <cmath>

extern Camera MainCamera;

// Camera.cpp との互換性を保つためのグローバル変数
// ※Camera.cpp を変更せずに参照できるようにここに定義しています
float marioSpeed;

int mario_centerX;
// デバッグ描画用のマリオの画面座標情報（screen座標）
int mario_debug_x1;
int mario_debug_y1;
int mario_debug_x2;
int mario_debug_y2;

// ==========================================
// 調整用の物理パラメータ（定数）
// ==========================================
const float MARIO_ACCEL = 0.4f;		// 1フレームごとの加速度（増やすとキレが良くなる）
const float MARIO_WALK_MAX_SPEED = 4.5f;	// 歩き状態の最高速度（これ以上速くならない）
const float MARIO_DASH_MAX_SPEED = 8.0f;   // ダッシュ状態の最高速度（これ以上速くならない）
const float MARIO_FRICTION = 0.3f;	// キーを離したときの摩擦・ブレーキ（減らすとよく滑る）
const float MARIO_DECEL_TURN = 0.8f;  // 逆キーを入れたときの急ブレーキの強さ

void Mario::ResolveCollision(Collidable& block)
{
	if (!collider.intersects(block.collider)) return;

	float overlapLeft = (collider.x + collider.width) - block.collider.x;
	float overlapRight = (block.collider.x + block.collider.width) - collider.x;
	float overlapTop = (collider.y + collider.height) - block.collider.y;
	float overlapBottom = (block.collider.y + block.collider.height) - collider.y;

	float minX = min(overlapLeft, overlapRight);
	float minY = min(overlapTop, overlapBottom);

	const float bias = 20.0f;

	if (minY < minX + bias)
	{
		if (overlapTop < overlapBottom)
		{
			position.y -= overlapTop;
			collider.y -= overlapTop;
			now_speed_y = 0;
			isJumping = false;
			jumpHoldTimer = 0.0f;
			block.OnHitTop(*this);
		}
		else
		{
			position.y += overlapBottom;
			collider.y += overlapBottom;
			now_speed_y = 0;
			block.OnHitBottom(*this);
		}
	}
	else
	{
		if (overlapLeft < overlapRight)
		{
			position.x -= overlapLeft;
			collider.x -= overlapLeft;
		}
		else
		{
			position.x += overlapRight;
			collider.x += overlapRight;
		}
		now_speed_x = 0;
		block.OnHitSide(*this);
	}
}


void Mario::Init()
{
	// 【あなたが追加したアニメーション初期化】
	walkAnimation.InitialAnimation(LoadGraph("data/mario/mario_walk_01.png"), 3, 15);

	// 【他の人が追加した画像と座標の初期化】
	marioImage.InitialImageAndSize(LoadGraph("data/image/mario_idle.png"));
	position.Set(165.0f, 772.0f);

	isLeft = false; // 最初は右向き
	now_speed_x = 0.0f; // 最初は静止している

	// Collider(コライダーの初期化)
	collider = Collider(position.x, position.y, marioImage.sizeX, marioImage.sizeY);
}

void Mario::Update()
{
	// image and position update
	marioImage.pos = position;

	// Collider update
	collider.x = position.x;
	collider.y = position.y;

	// LSHIFT + 方向キー の組み合わせは単独の方向キー判定より先に評価する (speed)
	if (CheckHitKey(KEY_INPUT_LSHIFT) && CheckHitKey(KEY_INPUT_D)) {
		marioSpeed = MARIO_DASH_MAX_SPEED; // ダッシュの最高速度（右向き）
	}
	else if (CheckHitKey(KEY_INPUT_D)) {
		marioSpeed = MARIO_WALK_MAX_SPEED; // 歩きの最高速度（右向き）
	}
	else if (CheckHitKey(KEY_INPUT_LSHIFT) && CheckHitKey(KEY_INPUT_A)) {
		marioSpeed = -MARIO_DASH_MAX_SPEED; // ダッシュの最高速度（左向き）
	}
	else if (CheckHitKey(KEY_INPUT_A)) {
		marioSpeed = -MARIO_WALK_MAX_SPEED; // 歩きの最高速度（左向き）
	}
	else {
		// キーが押されていないときは速度制御用変数は歩き相当にしておく（慣性で滑るため）
		marioSpeed = MARIO_WALK_MAX_SPEED;
	}

	// キー入力に応じた「速度（now_speed_x）」の計算
	if (CheckHitKey(KEY_INPUT_D) || (CheckHitKey(KEY_INPUT_LSHIFT) && CheckHitKey(KEY_INPUT_D))) // 歩きとダッシュの右移動
	{
		isLeft = false; // 右を向く

		if (now_speed_x < 0.0f)
		{
			now_speed_x += MARIO_DECEL_TURN; // 左に動いていたら急ブレーキ
		}
		else
		{
			now_speed_x += MARIO_ACCEL;      // 通常加速
		}
	}
	else if (CheckHitKey(KEY_INPUT_A) || (CheckHitKey(KEY_INPUT_LSHIFT) && CheckHitKey(KEY_INPUT_A))) // 歩きとダッシュの左移動
	{
		isLeft = true;  // 左を向く

		if (now_speed_x > 0.0f)
		{
			now_speed_x -= MARIO_DECEL_TURN; // 右に動いていたら急ブレーキ
		}
		else
		{
			now_speed_x -= MARIO_ACCEL;      // 通常加速
		}
	}
	else // 何も押していないとき
	{
		// 摩擦（自然減速）の処理
		if (now_speed_x > 0.0f)
		{
			now_speed_x -= MARIO_FRICTION;
			if (now_speed_x < 0.0f) now_speed_x = 0.0f; // 減速しすぎて逆走するのを防ぐ
		}
		else if (now_speed_x < 0.0f)
		{
			now_speed_x += MARIO_FRICTION;
			if (now_speed_x > 0.0f) now_speed_x = 0.0f; // 減速しすぎて逆走するのを防ぐ
		}
	}

	// 最高速度の制限（クランプ）
	float MarioMoovMaxSpeed = MARIO_WALK_MAX_SPEED;
	if (CheckHitKey(KEY_INPUT_LSHIFT) && (CheckHitKey(KEY_INPUT_D) || CheckHitKey(KEY_INPUT_A))) {
		MarioMoovMaxSpeed = MARIO_DASH_MAX_SPEED;
	}
	if (now_speed_x > MarioMoovMaxSpeed)  now_speed_x = MarioMoovMaxSpeed;
	if (now_speed_x < -MarioMoovMaxSpeed) now_speed_x = -MarioMoovMaxSpeed;

	// Dキーの押し下げに関係なく、マリオが画面中央を越えたらカメラを動かすように外に出しました
	float marioWorldCenterX = position.x + (marioImage.sizeX / 2.0f);
	if (marioWorldCenterX >= MainCamera.pos.x + (SCREEN_W / 2))
	{
		MainCamera.pos.x = marioWorldCenterX - (SCREEN_W / 2);
	}

	// カメラの移動可能範囲を制限 (0 ～ 12480)
	if (MainCamera.pos.x < 0) MainCamera.pos.x = 0;
	if (MainCamera.pos.x > 12480) MainCamera.pos.x = 12480;

	// マリオがカメラの左端より外に出ないようにする
	if (position.x < MainCamera.pos.x)
	{
		position.x = MainCamera.pos.x;
	}

	float scale = 4.0f;

	// マリオの表示幅を計算
	float marioWidth = marioImage.sizeX * scale; // マリオの画像の幅を計算
	float marioHeight = marioImage.sizeY * scale;// マリオの画像の高さを計算
	// マリオの右端のワールド座標
	float marioRightX = position.x + marioWidth;

	// ステージの右端を設定 (カメラの最大移動量 + 画面幅)
	float stageRightLimit = 12480.0f + SCREEN_W;

	// マリオがステージの右端を越えないように座標を補正
	if (marioRightX > stageRightLimit)
	{
		position.x = stageRightLimit - marioWidth;
	}

	// 画面上でのマリオの中心X座標を計算し、グローバル変数に保持する
	// ※Debug.cpp 側でこの情報を参照して当たり判定枠などを描画する
	int mario_screenX = (int)(position.x - MainCamera.pos.x);
	int mario_screenY = (int)(position.y - MainCamera.pos.y);
	mario_centerX = mario_screenX + ((int)marioWidth / 2);

	mario_debug_x1 = mario_screenX;
	mario_debug_y1 = mario_screenY;
	mario_debug_x2 = mario_screenX + (int)marioWidth;
	mario_debug_y2 = mario_screenY + (int)marioHeight;

	// 【あなたが追加したアニメーション処理】
	walkAnimation.x = (float)mario_screenX;
	walkAnimation.y = (float)mario_screenY;

	// 実際に「移動している速度」に基づいて walkAnimation の更新を行います
	if (std::abs(now_speed_x) > 0.1f)
	{
		walkAnimation.AnimationUpdateLoop(); // 正しく walkAnimation を更新
	}
	else
	{
		// 止まっているときは最初のコマ（立ちポーズ）に戻す
		walkAnimation.currentFrame = 0;
	}

	// 【他の人が追加したジャンプ処理】
	// first press — initial jump
	if (CheckHitKey(KEY_INPUT_SPACE) && !isJumping)
	{
		now_speed_y = JUMP_INITIAL;  // shoot up
		isJumping = true;
		jumpHoldTimer = 0.0f;
	}

	// hold to go higher
	if (CheckHitKey(KEY_INPUT_SPACE) && isJumping && jumpHoldTimer < JUMP_HOLD_MAX)
	{
		now_speed_y += JUMP_HOLD_FORCE;  // extra upward push
		jumpHoldTimer += 0.016f;         // add time (~1 frame at 60fps)
	}

	// RigidBody update (物理挙動の更新)
	PhysicsUpdate();
}

void Mario::Render()
{
	// 向きに応じて反転させて描画
	if (isLeft)
	{
		walkAnimation.AnimationRenderCenter(true); // 左向き (左右反転してアニメーション描画)
	}
	else
	{
		walkAnimation.AnimationRenderCenter(false); // 右向き (反転なしでアニメーション描画)
	}

	// DEBUG MODE
	if (map_mode == MODE_DEBUG)
	{
		DrawFormatString(0, 40, GetColor(255, 255, 255), "Mario pos X : %f, Mario pos Y : %f", position.x, position.y);
	}
}

//Jump player
void Mario::Jump()
{
	now_speed_y += JUMP_FORCE;
}