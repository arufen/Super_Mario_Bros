#include "Main.h"
#include "Game.h"
#include "Mario.h"
#include "Debug.h"
#include "Camera.h"

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
// NOTE: 他箇所で同名のシンボルが定義されている可能性があるため
// 定数名を一意にする（C2377 対策）。
<<<<<<< HEAD
const float MARIO_ACCEL = 0.4f;		// 1フレームごとの加速度（増やすとキレが良くなる）
const float MARIO_WALK_MAX_SPEED = 10.5f;	// 歩き状態の最高速度（これ以上速くならない）
const float MARIO_DASH_MAX_SPEED = 8.0f;   // ダッシュ状態の最高速度（これ以上速くならない）
const float MARIO_FRICTION = 0.3f;	// キーを離したときの摩擦・ブレーキ（減らすとよく滑る）
const float MARIO_DECEL_TURN = 0.8f;  // 逆キーを入れたときの急ブレーキの強さ
=======


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

>>>>>>> 30f87b127552e9cfc3a361a6981d4f865ff36fa6

void Mario::Init()
{
	// マリオの画像を読み込み、初期位置を設定
	marioImage.InitialImageAndSize(LoadGraph("data/mario_idle.png"));
	position.Set(165.0f, 700.0f); //772
	isLeft = false; // 最初は右向き

	now_speed_x = 0.0f; // 最初は静止している
	collider = Collider(position.x, position.y, marioImage.sizeX, marioImage.sizeY); // Collider(コライダーの初期化)
}

void Mario::Update()
{
	//image and position update
	marioImage.pos = position;

	//Collider update
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

    // 2. 最高速度の制限（クランプ）
	// 現在押されているキーに応じて最大速度を決定する（ダッシュ中は大きな値）
	float MarioMoovMaxSpeed = MARIO_WALK_MAX_SPEED;
	if (CheckHitKey(KEY_INPUT_LSHIFT) && (CheckHitKey(KEY_INPUT_D) || CheckHitKey(KEY_INPUT_A))) {
		MarioMoovMaxSpeed = MARIO_DASH_MAX_SPEED;
	}
	if (now_speed_x > MarioMoovMaxSpeed)  now_speed_x = MarioMoovMaxSpeed;
	if (now_speed_x < -MarioMoovMaxSpeed) now_speed_x = -MarioMoovMaxSpeed;

	// 3. 計算した速度を「実際の座標」に足し算する
	position.x += now_speed_x;

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

	// マリオの表示幅を計算
	float marioWidth = marioImage.sizeX; // マリオの画像の幅を計算
	float marioHeight = marioImage.sizeY;// マリオの画像の高さを計算
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
	// デバッグ描画用にマリオのスクリーン矩形を更新しておく
	// ※Debug.cpp 側でこの情報を参照して当たり判定枠などを描画する
	int mario_screenX = (int)(position.x - MainCamera.pos.x);
	int mario_screenY = (int)(position.y - MainCamera.pos.y);
	mario_centerX = mario_screenX + (marioWidth / 2);

	mario_debug_x1 = mario_screenX;
	mario_debug_y1 = mario_screenY;
	mario_debug_x2 = mario_screenX + marioWidth;
	mario_debug_y2 = mario_screenY + marioHeight;

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

	//// apply gravity always
	//now_speed_y += GRAVITY;

	//// move mario vertically
	//position.y += now_speed_y;

	//APPLY GRAVITY (重力）
	//To make sure doesn't go through collider when going
	//早く動くとき、当たり判定を通り抜けないために
	int steps = (int)(abs(now_speed_y) / (collider.height * 0.5f)) + 1;
	steps = min(steps, 10);

	for (int i = 0; i < steps; i++)
	{
		now_speed_y += GRAVITY / steps;
		position.y += now_speed_y / steps;
		collider.y = position.y;

		for (auto* block : collidables)
		{
			ResolveCollision(*block);
		}
	}

	// basic ground check (replace with ur real ground logic)
	/*if (position.y >= GROUND_Y)
	{
		position.y = GROUND_Y;
		now_speed_y = 0.0f;
		isJumping = false;
		jumpHoldTimer = 0.0f;
	}*/
	
}

void Mario::Render()
{
	// カメラの座標に合わせてマリオの描画位置（スクリーン座標）を計算
	int screenX = (int)(marioImage.pos.x - MainCamera.pos.x);
	int screenY = (int)(marioImage.pos.y - MainCamera.pos.y);
	
	// 向きに応じて反転させて描画 (画像の左上を基準点として等倍で描画)
	if (isLeft)
	{
		// 左向き (左右反転)
		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, marioImage.image, TRUE, TRUE);
	}
	else
	{
		// 右向き (反転なし)
		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, marioImage.image, TRUE, FALSE);
	}


	//DEBUG MODE
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