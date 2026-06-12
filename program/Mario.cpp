#include "Main.h"
#include "Game.h"
#include "Mario.h"
#include "Debug.h"
#include "Camera.h"
#include "StageManager.h"
#include "Sound.h"

extern Camera MainCamera;

// Camera.cpp との互換性を保つためのグローバル変数
float marioSpeed;
int mario_centerX;

// デバッグ描画用のマリオの画面座標情報（screen座標）
int small_mario_debug_x1;
int small_mario_debug_y1;
int small_mario_debug_x2;
int small_mario_debug_y2;

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
	// 死亡時または土管移動中は衝突判定を一切行わない
	if (currentState == MarioState::DEAD || currentState == MarioState::WARPING) return;

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

	float bias = 0.5f;
	if (dynamic_cast<Enemy*>(&block) != nullptr)
	{
		bias = 14.0f; // 10.0f〜15.0f 程度にすると「理不尽な横死」がなくなります
	}

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

// 敵に接触したときなどに呼び出される死亡開始関数
void Mario::ToDeadState(bool isFall)
{
	if (currentState == MarioState::DEAD) return;

	if (currentForm == MarioForm::SMALL)
	{
		currentState = MarioState::DEAD;
		isFellDown = isFall;
		warpTimer = 0.0f;

		now_speed_x = 0.0f;
		now_speed_y = 0.0f;

		// 当たり判定を完全に無くしてステージや敵をすり抜けさせる
		//RigidBody_collider.x = -9999.0f;
		//RigidBody_collider.y = -9999.0f;
		RigidBody_collider.width = 0;
		RigidBody_collider.height = 0;

		// 死亡BGMを鳴らす
		SoundManager::GetInstance().PlayDeadBGM();
	}
	else
	{
		// 将来的にスーパーマリオなどの場合は、ここにFormをSMALLに落とす処理を記述
	}
}

void Mario::Init()
{
	// マリオの画像を読み込み、初期位置を設定
	small_mario_waitImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_wait.png"));
	small_mario_jumpImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_jump.png"));
	small_mario_deadImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_dead.png"));
	small_mario_walkAnim.InitialAnimation(LoadGraph("data/image/mario/small_mario_walk.png"), 3, 10);

	currentForm = MarioForm::SMALL;
	position.Set(165.0f, 700.0f); //772
	isLeft = false; // 最初は右向き
	isDeadJumped = false;
	isFellDown = false;

	now_speed_x = 0.0f; // 最初は静止している
	now_speed_y = 0.0f;//new added for warp pipe
	currentState = MarioState::NORMAL;
	warpTimer = 0.0f;

	//Shrink collider (当たり判定を画像より少し小さくする)
	RigidBody_collider = Collider(position.x, position.y, small_mario_waitImage.sizeX, small_mario_waitImage.sizeY);
	float offsetWidth = 20.0f; // how much to shrink total width
	RigidBody_collider.width -= offsetWidth;
	RigidBody_collider.x += offsetWidth / 2; // shift right so it's centered

	//Score and item
	score = 0;
	coin = 0;

	//マリオのstar状態のアニメーションの初期化
	int handle = LoadGraph("data/image/starmario_Animation_02.png");
	starmarioAnim.InitialAnimation(handle, 3, 10); // スター画像は3フレームなので 3 を指定
	isStarMode = false;
	starTimer = 0.0f;

}

void Mario::Update()
{
	// ----------------------------------------------------
	// 死亡状態（DEAD）の更新処理
	// ----------------------------------------------------
	if (currentState == MarioState::DEAD)
	{
		warpTimer += 0.016f;

		// 落下死の場合は、跳ね上がり計算をすべてスキップして終了（消滅状態を維持）
		if (isFellDown)
		{
			now_speed_x = 0.0f;
			now_speed_y = 0.0f;

			// デバッグ用矩形も画面外へ飛ばしておく
			small_mario_debug_x1 = -9999;
			small_mario_debug_y1 = -9999;
			small_mario_debug_x2 = -9999;
			small_mario_debug_y2 = -9999;
			return;
		}

		if (warpTimer >= 0.5f)
		{
			// 0.5秒経った最初の1フレームだけ、上向きの初速を与える
			if (!isDeadJumped)
			{
				now_speed_y = -12.0f; // 上向きの初速
				isDeadJumped = true;  // 跳ねたのでフラグを立てる（二度と入らない）
			}

			now_speed_y += 0.5f; // 死亡時専用の擬似重力（下向きに加速）
			position.y += now_speed_y;
		}
		else
		{
			now_speed_y = 0.0f;
		}

		// 死亡画像の座標をマリオの現在地に更新
		small_mario_deadImage.pos = position;

		// デバッグ用の矩形範囲も現在の位置に合わせて更新
		int scrX1 = (int)(RigidBody_collider.x - MainCamera.pos.x);
		int scrY1 = (int)(RigidBody_collider.y - MainCamera.pos.y);
		small_mario_debug_x1 = scrX1;
		small_mario_debug_y1 = scrY1;
		small_mario_debug_x2 = scrX1 + (int)RigidBody_collider.width;
		small_mario_debug_y2 = scrY1 + (int)RigidBody_collider.height;
		mario_centerX = scrX1 + ((int)RigidBody_collider.width / 2);

		return;
	}

	// ----------------------------------------------------
	// 土管ワープ状態（WARPING）の更新処理
	// ----------------------------------------------------
	if (currentState == MarioState::WARPING)
	{
		warpTimer += 0.016f; 
		
		if (warpTimer < WARP_DURATION)
		{
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
		}

		now_speed_x = 0.0f;
		now_speed_y = 0.0f;

		small_mario_waitImage.pos = position;
		RigidBody_collider.x = position.x;
		RigidBody_collider.y = position.y;

		// Frame debug layout configuration parameters
		int scrX1 = (int)(RigidBody_collider.x - MainCamera.pos.x);
		int scrY1 = (int)(RigidBody_collider.y - MainCamera.pos.y);
		small_mario_debug_x1 = scrX1;
		small_mario_debug_y1 = scrY1;
		small_mario_debug_x2 = scrX1 + (int)RigidBody_collider.width;
		small_mario_debug_y2 = scrY1 + (int)RigidBody_collider.height;
		mario_centerX = scrX1 + ((int)RigidBody_collider.width / 2);

		if (warpTimer >= WARP_DURATION)
		{
			if (SoundManager::GetInstance().IsPlayingBGM("HurryAction"))
			{
				return;
			}

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

	// ----------------------------------------------------
	// 通常状態（NORMAL）の更新処理
	// ----------------------------------------------------
	// デバッグモードでPキーが押されたらスター状態にする
	if (map_mode == MODE_DEBUG)
	{
		if (CheckHitKey(KEY_INPUT_P))
		{
			Star();
		}
	}
	// 奈落への落下死亡チェック
	// 現在のステージ（ゾーン）によって判定する高さを変える
	float currentDeadLineY = (StageManager::GetInstance().GetWorldZone() == WorldZone::OVERWORLD)
		? DEAD_LINE_OVERWORLD
		: DEAD_LINE_UNDERWORLD;

	if (position.y > currentDeadLineY)
	{
		ToDeadState(true);
		return; // 死亡状態に移行したのでここで処理を抜ける
	}

	//image and position update
	small_mario_waitImage.pos = position;

	////Collider update
	float offsetWidth = 20.0f;
	RigidBody_collider.x = position.x + offsetWidth / 2;
	RigidBody_collider.y = position.y;
	//collider.x = position.x + 20.0f / 2;
	//collider.y = position.y;
	
	bool currKeyA = CheckHitKey(KEY_INPUT_A);
	bool currKeyD = CheckHitKey(KEY_INPUT_D);
	bool isDashing = CheckHitKey(KEY_INPUT_LSHIFT);
	bool currKeySpace = CheckHitKey(KEY_INPUT_SPACE);

	// 後入力優先の方向決定ロジック (ID#a0004から復活)
	if (currKeyA && !prevKeyA) preferLeftInput = true;
	if (currKeyD && !prevKeyD) preferLeftInput = false;
	if (currKeyA && !currKeyD) preferLeftInput = true;
	if (currKeyD && !currKeyA) preferLeftInput = false;

	prevKeyA = currKeyA;
	prevKeyD = currKeyD;

	bool moveKeyPressed = currKeyA || currKeyD;

	// 外部参照用(Camera.cpp等)の移動速度設定
	if (moveKeyPressed)
	{
		float currentMax = isDashing ? MARIO_DASH_MAX_SPEED : MARIO_WALK_MAX_SPEED;
		marioSpeed = preferLeftInput ? -currentMax : currentMax;
	}
	else
	{
		marioSpeed = MARIO_WALK_MAX_SPEED;
	}

	// キー入力に応じた速度（now_speed_x）と向きの計算 (後入力優先ベース)
	if (moveKeyPressed)
	{
		if (preferLeftInput)
		{
			if (!isJumping) isLeft = true;

			if (now_speed_x > 0.0f)	now_speed_x -= MARIO_DECEL_TURN; // 急ブレーキ
			else					now_speed_x -= MARIO_ACCEL;      // 通常加速
		}
		else
		{
			if (!isJumping) isLeft = false;

			if (now_speed_x < 0.0f)	now_speed_x += MARIO_DECEL_TURN; // 急ブレーキ
			else					now_speed_x += MARIO_ACCEL;      // 通常加速
		}
	}
	else
	{
		// 摩擦（自然減速）の処理
		if (now_speed_x > 0.0f)
		{
			now_speed_x -= MARIO_FRICTION;
			if (now_speed_x < 0.0f) now_speed_x = 0.0f;
		}
		else if (now_speed_x < 0.0f)
		{
			now_speed_x += MARIO_FRICTION;
			if (now_speed_x > 0.0f) now_speed_x = 0.0f;
		}
	}

	// 最高速度の制限（クランプ）
	float MarioMoovMaxSpeed = isDashing ? MARIO_DASH_MAX_SPEED : MARIO_WALK_MAX_SPEED;
	if (now_speed_x > MarioMoovMaxSpeed)  now_speed_x = MarioMoovMaxSpeed;
	if (now_speed_x < -MarioMoovMaxSpeed) now_speed_x = -MarioMoovMaxSpeed;

	// Dキーの押し下げに関係なく、マリオが画面中央を越えたらカメラを動かすように外に出しました
	if (MainCamera.pos.y < 960)
	{
		float marioWorldCenterX = position.x + (small_mario_waitImage.sizeX / 2.0f);
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
	float marioWidth = small_mario_waitImage.sizeX;  // マリオの画像の幅を計算
	float marioHeight = small_mario_waitImage.sizeY; // マリオの画像の高さを計算
	// マリオの右端のワールド座標
	float marioRightX = position.x + marioWidth;

	// ステージの右端を設定 (カメラの最大移動量 + 画面幅)
	float stageRightLimit = 12480.0f + SCREEN_W;

	// first press — initial jump
	if (currKeySpace && !prevKeySpace && !isJumping)
	{
		Jump();
	}

	// hold to go higher
	if (currKeySpace && isJumping && jumpHoldTimer < JUMP_HOLD_MAX)
	{
		now_speed_y += JUMP_HOLD_FORCE;  // extra upward push
		jumpHoldTimer += 0.016f;         // add time (~1 frame at 60fps)
	}

	// 次のフレームのために現在の状態を過去の状態として保存
	prevKeySpace = currKeySpace;

	// 歩きアニメーションのループ更新処理 (ID#a0004から復活)
	if (moveKeyPressed && !isJumping)
	{
		isWalking = true;
		small_mario_walkAnim.FPS = isDashing ? 15 : 9;
		small_mario_walkAnim.AnimationUpdateLoop();
	}
	else
	{
		isWalking = false;
		small_mario_walkAnim.currentFrame = 0;
	}

	//RigidBody update
	PhysicsUpdate();

	// 物理演算（PhysicsUpdate）が終わった直後に右端のストッパー処理を入れる

	if (position.x + marioWidth > stageRightLimit)
	{
		position.x = stageRightLimit - marioWidth; // 座標を右端限界にクランプ

		// 実際の当たり判定（RigidBody_collider）のX座標も即座に再同期させる
		float offsetWidth = 20.0f;
		RigidBody_collider.x = position.x + offsetWidth / 2;

		// 右へ進もうとする慣性速度をゼロにする
		if (now_speed_x > 0.0f) now_speed_x = 0.0f;
	}

	// 衝突応答がすべて終わった最新の RigidBody_collider からデバッグ用スクリーン座標を計算
	int scrX1 = (int)(RigidBody_collider.x - MainCamera.pos.x);
	int scrY1 = (int)(RigidBody_collider.y - MainCamera.pos.y);

	small_mario_debug_x1 = scrX1;
	small_mario_debug_y1 = scrY1;
	small_mario_debug_x2 = scrX1 + (int)RigidBody_collider.width;
	small_mario_debug_y2 = scrY1 + (int)RigidBody_collider.height;

	// 画面中央線や中心線の描画に使うマリオの中心X座標も、実際の当たり判定の中心にする
	mario_centerX = scrX1 + ((int)RigidBody_collider.width / 2);

	// スター状態の更新処理
	if (isStarMode)
	{
		starTimer -= 1.0f;
		if (starTimer <= 0.0f)
		{
			isStarMode = false; // 時間切れで通常状態に戻る
		}

		// アニメーションの更新
		starmarioAnim.AnimationUpdateLoop();
	}
}

void Mario::Render()
{
	// カメラの座標に合わせてマリオの描画位置（スクリーン座標）を計算
	int screenX = (int)(position.x - MainCamera.pos.x);
	int screenY = (int)(position.y - MainCamera.pos.y);
	
	// 死亡時は最優先でデッド画像を描画
	if (currentState == MarioState::DEAD)
	{
		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_deadImage.image, TRUE, FALSE);
		return;
	}

	//  土管ワープ中は強制的に待機（wait）ポーズにする
	if (currentState == MarioState::WARPING)
	{
		// 土管に入るときの向き（isLeft）に合わせて反転を考慮しつつ、待機画像を描画
		if (isLeft)	DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_waitImage.image, TRUE, TRUE);
		else		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_waitImage.image, TRUE, FALSE);
		return; // ワープ中はここで描画処理を終了する（下の歩きやジャンプを通さない）
	}

	if (isStarMode)
	{
		// スター状態なら専用のアニメーションを優先して描画する
		starmarioAnim.x = (float)screenX;
		starmarioAnim.y = (float)screenY;
		starmarioAnim.AnimationRenderCenter(isLeft);
	}
	else {
		if (isJumping)
		{
			if (isLeft)	DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_jumpImage.image, TRUE, TRUE);
			else		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_jumpImage.image, TRUE, FALSE);
		}
		else if (isWalking)
		{
			small_mario_walkAnim.x = (float)screenX;
			small_mario_walkAnim.y = (float)screenY;
			small_mario_walkAnim.AnimationRenderCenter(isLeft);
		}
		else
		{
			if (isLeft)	DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_waitImage.image, TRUE, TRUE);
			else		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_waitImage.image, TRUE, FALSE);
		}
	}

	// DEBUG MODE
	if (map_mode == MODE_DEBUG)
	{
		DrawFormatString(0, 40, GetColor(255, 255, 255), "Mario pos X : %f, Mario pos Y : %f", position.x, position.y);
		DrawFormatString(0, 60, GetColor(255, 255, 255), "now_speed X : %f", now_speed_x);

		//スター状態の残り時間を表示
		if (isStarMode) {
			DrawFormatString(0, 80, GetColor(255, 255, 0), "STAR MODE: %.0f", starTimer);
		}
		else {
			DrawFormatString(0, 80, GetColor(100, 100, 100), "STAR MODE: OFF");
		}
	}
}

//Jump player
void Mario::Jump()
{
	now_speed_y = JUMP_INITIAL;  // shoot up
	isJumping = true;
	jumpHoldTimer = 0.0f;
	SoundManager::GetInstance().PlaySE("Jump_Small");
}

void Mario::AddCoin()
{
	coin++;
}

void Mario::Star()
{
	isStarMode = true;
	starTimer = 600.0f; // 例: 60fps環境で10秒間 (60 * 10)
}