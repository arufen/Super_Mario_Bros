#include "Main.h"
#include "Game.h"
#include "Mario.h"
#include "Debug.h"
#include "Camera.h"
#include "StageManager.h"
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
const float MARIO_ACCEL = 0.4f;		// 1フレームごとの加速度（増やすとキレが良くなる）
const float MARIO_WALK_MAX_SPEED = 10.5f;	// 歩き状態の最高速度（これ以上速くならない）
const float MARIO_DASH_MAX_SPEED = 8.0f;   // ダッシュ状態の最高速度（これ以上速くならない）
const float MARIO_FRICTION = 0.3f;	// キーを離したときの摩擦・ブレーキ（減らすとよく滑る）
const float MARIO_DECEL_TURN = 0.8f;  // 逆キーを入れたときの急ブレーキの強さ


void Mario::ResolveCollision(Collidable& block)
{
	if (currentState == MarioState::WARPING) return;

	if (!RigidBody_collider.intersects(block.collider)) return;

	//for isTrigger Collision
	if (block.isTrigger)
	{
		//Coin
		Coin* coin = dynamic_cast<Coin*>(&block);
		if (coin != nullptr)
		{
			AddCoin();
			coin->active = false; // Mark the coin as collected
		}

		block.OnHitSide(*this);  // just calls callback, no pushing
		return;
	}

	float overlapLeft = (RigidBody_collider.x + RigidBody_collider.width) - block.collider.x;
	float overlapRight = (block.collider.x + block.collider.width) - RigidBody_collider.x;
	float overlapTop = (RigidBody_collider.y + RigidBody_collider.height) - block.collider.y;
	float overlapBottom = (block.collider.y + block.collider.height) - RigidBody_collider.y;

	float minX = min(overlapLeft, overlapRight);
	float minY = min(overlapTop, overlapBottom);

	const float bias = 0.5f;

	if (minY < minX + bias)
	{

		if (hitVertical) return;

		if (overlapTop < overlapBottom)
		{
			//Adjust mario position
			position.y -= overlapTop;
			RigidBody_collider.y -= overlapTop;
			now_speed_y = 0;
			isJumping = false;
			jumpHoldTimer = 0.0f;
			hitVertical = true;  // mark it

			//jump if hit enemy on top (like raycast)
			Enemy* enemy = dynamic_cast<Enemy*>(&block);
			if (enemy != nullptr)
			{
				Jump();
				/*enemy->TakeDamage(*this);*/
			}

			
			block.OnHitTop(*this);
		}
		else
		{
			position.y += overlapBottom;
			RigidBody_collider.y += overlapBottom;
			now_speed_y = 0;

			// find closest block to mario center
			float marioCX = RigidBody_collider.x + RigidBody_collider.width / 2.0f;
			float marioCY = RigidBody_collider.y + RigidBody_collider.height / 2.0f;

			Collidable* closest = nullptr;
			float closestDist = 999999.0f;

			for (auto* c : collidables)
			{
				if (!RigidBody_collider.intersects(c->collider)) continue;
				float cx = c->collider.x + c->collider.width / 2.0f;
				float cy = c->collider.y + c->collider.height / 2.0f;
				float dist = (marioCX - cx) * (marioCX - cx) + (marioCY - cy) * (marioCY - cy);
				if (dist < closestDist)
				{
					closestDist = dist;
					closest = c;
				}
			}

			// only trigger item/break on closest block
			if (closest == &block)
			{
				QuestionBlock* questionBlock = dynamic_cast<QuestionBlock*>(&block);
				if (questionBlock != nullptr && questionBlock->IsAvailable())
				{
					switch (questionBlock->itemType)
					{
					case QuestionBlockItem::COIN:
						AddCoin();
						break;
					}
				}

				block.OnHitBottom(*this);
			}
		}
	}
	else
	{
		if (overlapLeft < overlapRight)
		{
			position.x -= overlapLeft;
			RigidBody_collider.x -= overlapLeft;
		}
		else
		{
			position.x += overlapRight;
			RigidBody_collider.x += overlapRight;
		}

		now_speed_x = 0;

		block.OnHitSide(*this);
	}
}

//void Mario::Warping(float pipeY)
//{
//	currentState = MarioState::WARPING;
//	warpTimer = 0.0f;
//
//	now_speed_x = 0.0f;
//	now_speed_y = 0.0f;
//
//	position.y = pipeY;
//}
void Mario::Warping(float targetX, float targetY, WarpDir dir)
{
	currentState = MarioState::WARPING;
	currentWarpDir = dir; // Store the direction
	warpTimer = 0.0f;

	now_speed_x = 0.0f;
	now_speed_y = 0.0f;

	// Set Mario to the starting position of the animation
	position.x = targetX;
	position.y = targetY;
}
void Mario::Init()
{
	// マリオの画像を読み込み、初期位置を設定
	marioImage.InitialImageAndSize(LoadGraph("data/image/mario_idle.png"));
	position.Set(165.0f, 700.0f); //772
	isLeft = false; // 最初は右向き

	now_speed_x = 0.0f; // 最初は静止している
	now_speed_y = 0.0f;//new added for warp pipe
	currentState = MarioState::NORMAL;
	warpTimer = 0.0f;
	RigidBody_collider = Collider(position.x, position.y, marioImage.sizeX, marioImage.sizeY); // Collider(コライダーの初期化)

	//Shrink collider (当たり判定を画像より少し小さくする)
	float offsetWidth = 20.0f; // how much to shrink total width
	RigidBody_collider.width -= offsetWidth;
	RigidBody_collider.x += offsetWidth / 2; // shift right so it's centered

	//Score and item
	score = 0;
	coin = 0;
}

void Mario::Update()
{
	if (currentState == MarioState::WARPING)
	{
		warpTimer += 0.016f; 
		
		if (currentWarpDir == WarpDir::DOWN)
		{
			position.y += 1.5f;
		}
		else if (currentWarpDir == WarpDir::RIGHT)
		{
			position.x += 1.5f;
		}
		else if (currentWarpDir == WarpDir::UP)
		{
			position.y -= 1.5f;
		}

		now_speed_x = 0.0f;
		now_speed_y = 0.0f;

		marioImage.pos = position;
		RigidBody_collider.x = position.x;
		RigidBody_collider.y = position.y;

		// Frame debug layout configuration parameters
		int mScrX = (int)(position.x - MainCamera.pos.x);
		int mScrY = (int)(position.y - MainCamera.pos.y);
		mario_centerX = mScrX + (marioImage.sizeX / 2);
		mario_debug_x1 = mScrX;
		mario_debug_y1 = mScrY;
		mario_debug_x2 = mScrX + marioImage.sizeX;
		mario_debug_y2 = mScrY + marioImage.sizeY;

		if (warpTimer >= WARP_DURATION)
		{
			currentState = MarioState::NORMAL;

			if (currentWarpDir == WarpDir::DOWN)
			{
				// the Overworld pipe -> Underworld
				StageManager::GetInstance().TransferWorldZone(WorldZone::UNDERWORLD);
			}
			else if (currentWarpDir == WarpDir::RIGHT)
			{
				//  the Underworld side pipe -> Overworld
				StageManager::GetInstance().TransferWorldZone(WorldZone::OVERWORLD);
			}
			else if (currentWarpDir == WarpDir::UP)
			{
				//rising UP out of the Overworld pipe!
			}
		}
		return;
	}
	//image and position update
	marioImage.pos = position;

	////Collider update
	float offsetWidth = 20.0f;
	RigidBody_collider.x = position.x + offsetWidth / 2;
	//collider.x = position.x + 20.0f / 2;
	//collider.y = position.y;
	

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

	//// 3. 計算した速度を「実際の座標」に足し算する
	//position.x += now_speed_x;

	// Dキーの押し下げに関係なく、マリオが画面中央を越えたらカメラを動かすように外に出しました
	if(MainCamera.pos.y < 960)
	{ 
	float marioWorldCenterX = position.x + (marioImage.sizeX / 2.0f);
	if (marioWorldCenterX >= MainCamera.pos.x + (SCREEN_W / 2))
	{
		MainCamera.pos.x = marioWorldCenterX - (SCREEN_W / 2);
	}
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
		Jump();
	}

	// hold to go higher
	if (CheckHitKey(KEY_INPUT_SPACE) && isJumping && jumpHoldTimer < JUMP_HOLD_MAX)
	{
		now_speed_y += JUMP_HOLD_FORCE;  // extra upward push
		jumpHoldTimer += 0.016f;         // add time (~1 frame at 60fps)
	}

	//RigidBody update
	PhysicsUpdate();
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
		DrawFormatString(0, 60, GetColor(255, 255, 255), "now_speed X : %f", now_speed_x);
	}
	}

	

//Jump player
void Mario::Jump()
{
	now_speed_y = JUMP_INITIAL;  // shoot up
	isJumping = true;
	jumpHoldTimer = 0.0f;
}

void Mario::AddCoin()
{
	coin++;
}