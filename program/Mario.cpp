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

		//Collect item
		CollectableItem* item = dynamic_cast<CollectableItem*>(&block);
		if (item != nullptr)
		{
			if (item->canCollect)
			{
				item->CollectItem(*this);
			}
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

	if (now_speed_y >= 0.0f && overlapTop < overlapBottom && minX > 4.0f && overlapTop <= 8.0f)
	{
		if (hitVertical) return;

		// 床（着地）処理
		position.y -= overlapTop;
		RigidBody_collider.y -= overlapTop;
		now_speed_y = 0;
		isJumping = false;
		jumpHoldTimer = 0.0f;
		hitVertical = true;  // mark it

		block.OnHitTop(*this);
		return; // 横の壁判定（speed_x = 0）に絶対に流さないようにここで終了
	}

	float bias = 0.5f;
	if (dynamic_cast<Enemy*>(&block) != nullptr)
	{
		bias = 14.0f; // 敵に対しては「理不尽な横死」を防ぐために大きめのバイアスを維持
	}
	else
	{
		// ブロックや地形に対する処理
		if (!isJumping)
		{
			// 地上にいる時は、床の継ぎ目で引っかからないようにバイアスを大きくして床判定（縦）を優先する
			bias = 5.0f;
		}
		else
		{
			// 空中（ジャンプ中）に壁にぶつかった時は、頭突きと誤認させないようにバイアスを小さくして壁判定（横）を優先する
			bias = 0.2f;
		}
	}

	bool isBrushingCornerWhileRising = (now_speed_y < 0.0f && overlapTop < overlapBottom);
	if (minY < minX + bias && !isBrushingCornerWhileRising)
	{
		if (hitVertical) return;

		if (overlapTop < overlapBottom)
		{
			// 【安全弁】ここを通るのは now_speed_y >= 0.0f のときだけ
			if (now_speed_y >= 0.0f)
			{
				position.y -= overlapTop;
				RigidBody_collider.y -= overlapTop;
				now_speed_y = 0;
				isJumping = false;
				jumpHoldTimer = 0.0f;
				hitVertical = true;

				block.OnHitTop(*this);
			}
		}
		else
		{
			if (now_speed_y < 0.0f)
			{
				// 完全に真下から天井に頭をぶつけた時の処理
				position.y += overlapBottom;
				RigidBody_collider.y += overlapBottom;

				now_speed_y = 0;
				jumpHoldTimer = JUMP_HOLD_MAX; // ホールド強制終了
			}

			// 最も近いブロックの特定とアイテム・破壊処理
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

			if (closest == &block)
			{
				QuestionBlock* questionBlock = dynamic_cast<QuestionBlock*>(&block);
				if (questionBlock != nullptr && questionBlock->IsAvailable())
				{
					switch (questionBlock->itemType)
					{
					case QuestionBlockItem::COIN: AddCoin(); break;
					}
				}
				BrickBlock* brickBlock = dynamic_cast<BrickBlock*>(&block);
				if (brickBlock != nullptr && brickBlock->IsAvailable())
				{
					switch (brickBlock->itemType)
					{
					case BrickBlockItem::COIN: AddCoin(); break;
					}
				}
				HiddenBlock* hiddenBlock = dynamic_cast<HiddenBlock*>(&block);
				if (hiddenBlock != nullptr && hiddenBlock->IsRevealed())
				{
					switch (hiddenBlock->itemType)
					{
					case HiddenItemType::COIN: AddCoin(); break;
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

	if (!isFall)
	{
		if (invincibleTimer.GetCurrentTimer() > 0.0f) return;
		if (starEffect.isActive) return;
	}

	if (isFall || currentForm == MarioForm::SMALL)
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
	big_mario_waitImage.InitialImageAndSize(LoadGraph("data/image/big_mario_wait.png"));
	big_mario_fire_waitImage.InitialImageAndSize(LoadGraph("data/image/big_mario_fire_wait.png"));


	currentForm = MarioForm::SMALL;
	position.Set(165.0f, 700.0f);
	isLeft = false; // 最初は右向き
	isDeadJumped = false;
	isFellDown = false;

	now_speed_x = 0.0f; // 最初は静止している
	now_speed_y = 0.0f;//new added for warp pipe
	currentState = MarioState::NORMAL;
	warpTimer = 0.0f;

	//Shrink collider (当たり判定を画像より少し小さくする)
	RigidBody_collider = Collider(position.x, position.y, small_mario_waitImage.sizeX, small_mario_waitImage.sizeY);
	RigidBody_collider.width -= COLLIDER_OFFSET;
	RigidBody_collider.x += COLLIDER_OFFSET / 2; // shift right so it's centered

	//Score and item
	score = 0;
	coin = 0;

	//マリオのstar状態のアニメーションの初期化
	starEffect.Init();

	//For debug (Debug用)
	debug_is_inivincible = false;
}

void Mario::Update()
{
	//For debug(DEBUG用）
	if (map_mode == MODE_DEBUG && CheckHitKey(KEY_INPUT_LCONTROL) && PushHitKey(KEY_INPUT_I))
	{
		debug_is_inivincible = !debug_is_inivincible ? true  : false;
	}
	if(debug_is_inivincible) invincibleTimer.SetTimer(1.0f);
	

	invincibleTimer.Update();
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
	// カットシーン状態（CUTSCENE）の更新処理
	// ----------------------------------------------------
	if (currentState == MarioState::CUTSCENE)
	{
		if (!StageManager::GetInstance().isBossDefeat) return;
		// StartCutsceneWalk() set the target once, now walk toward it every frame
		WalkTo(cutsceneTargetX, cutsceneSpeed);

		RigidBody_collider.x = position.x + COLLIDER_OFFSET / 2;
		RigidBody_collider.y = position.y;

		int scrX1 = (int)(RigidBody_collider.x - MainCamera.pos.x);
		int scrY1 = (int)(RigidBody_collider.y - MainCamera.pos.y);
		small_mario_debug_x1 = scrX1;
		small_mario_debug_y1 = scrY1;
		small_mario_debug_x2 = scrX1 + (int)RigidBody_collider.width;
		small_mario_debug_y2 = scrY1 + (int)RigidBody_collider.height;
		mario_centerX = scrX1 + ((int)RigidBody_collider.width / 2);

		//Not moving when bridge is clearing
		/*if(!StageManager::GetInstance().isBridgeClearing)*/ PhysicsUpdate();

		// Dキーの押し下げに関係なく、マリオが画面中央を越えたらカメラを動かすように外に出しました
		if (MainCamera.pos.y < 960)
		{
			float marioWorldCenterX = position.x + (small_mario_waitImage.sizeX / 2.0f);
			if (marioWorldCenterX >= MainCamera.pos.x + (SCREEN_W / 2))
			{
				MainCamera.pos.x = marioWorldCenterX - (SCREEN_W / 2);
			}
		}

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
		//RigidBody_collider.x = position.x;
		//RigidBody_collider.y = position.y;

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
			// スター状態開始
			starEffect.Start();
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
	RigidBody_collider.x = position.x + COLLIDER_OFFSET / 2;
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

	// 地上と空中で加速度や摩擦を切り替える（空中慣性を再現）
	float activeAccel = isJumping ? MARIO_AIR_ACCEL : MARIO_ACCEL;
	float activeFriction = isJumping ? MARIO_AIR_FRICTION : MARIO_FRICTION;
	float activeTurn = isJumping ? MARIO_AIR_ACCEL : MARIO_DECEL_TURN; // 空中では急ブレーキが効かない

	// 最高速度の決定
	float maxSpeed = isDashing ? MARIO_DASH_MAX_SPEED : MARIO_WALK_MAX_SPEED;

	// キー入力に応じた速度（now_speed_x）と向きの計算
	if (moveKeyPressed)
	{
		if (preferLeftInput)
		{
			if (!isJumping) isLeft = true;

			if (now_speed_x > 0.0f)	now_speed_x -= activeTurn;
			else					now_speed_x -= activeAccel;
		}
		else
		{
			if (!isJumping) isLeft = false;

			if (now_speed_x < 0.0f)	now_speed_x += activeTurn;
			else					now_speed_x += activeAccel;
		}
	}
	else
	{
		if (now_speed_x > 0.0f)
		{
			now_speed_x -= activeFriction;
			if (now_speed_x < 0.0f) now_speed_x = 0.0f;
		}
		else if (now_speed_x < 0.0f)
		{
			now_speed_x += activeFriction;
			if (now_speed_x > 0.0f) now_speed_x = 0.0f;
		}
	}

	// 最高速度の制限（クランプ）
	if (now_speed_x > maxSpeed)  now_speed_x = maxSpeed;
	if (now_speed_x < -maxSpeed) now_speed_x = -maxSpeed;

	// 外部参照用（Camera.cppなど）に現在の速度を同期
	marioSpeed = now_speed_x;

	// Dキーの押し下げに関係なく、マリオが画面中央を越えたらカメラを動かすように外に出しました
	if (MainCamera.pos.y < 960)
	{
		float marioWorldCenterX = position.x + (small_mario_waitImage.sizeX / 2.0f);
		if (marioWorldCenterX >= MainCamera.pos.x + (SCREEN_W / 2))
		{
			MainCamera.pos.x = marioWorldCenterX - (SCREEN_W / 2);
		}
	}

	// マリオがカメラの左端より外に出ないようにする
	if (position.x < MainCamera.pos.x)
	{
		position.x = MainCamera.pos.x;
	}

	// マリオの表示幅を計算
	float marioWidth = small_mario_waitImage.sizeX;

	// ステージの右端を設定 (カメラの最大移動量 + 画面幅)
	float stageRightLimit = 12480.0f + SCREEN_W;

	// 足を踏み外して勝手に落下し始めた場合、強制的に空中状態(isJumping=true)にする
	if (!isJumping && now_speed_y > 0.0f)
	{
		isJumping = true;
	}

	// first press — initial jump
	if (currKeySpace && !prevKeySpace && !isJumping && now_speed_y == 0.0f)
	{
		Jump();
	}

	// hold to go higher
	if (currKeySpace && isJumping && jumpHoldTimer > 0.0f && jumpHoldTimer < JUMP_HOLD_MAX)
	{
		now_speed_y += JUMP_HOLD_FORCE;  // extra upward push
		jumpHoldTimer += 0.016f;         // add time (~1 frame at 60fps)
	}

	// 次のフレームのために現在の状態を過去の状態として保存
	prevKeySpace = currKeySpace;

	// ----------------------------------------------------
	// 速度に応じたアニメーション速度（FPS）の動的計算
	// ----------------------------------------------------
	// 現在のリアルな横移動スピード（絶対値）を取得
	float currentAbsSpeedX = abs(now_speed_x);
	int currentWalkFPS = 0;

	bool isActualJumpingPose = isJumping && (jumpHoldTimer > 0.0f || now_speed_y < 0.0f);

	if (moveKeyPressed && !isActualJumpingPose)
	{
		isWalking = true;

		// 速度0.0f〜最高速(7.5f)に応じてFPSを3〜16の間でなめらかに動的変化させる公式
		// 停止寸前はゆっくりバタバタ、ダッシュ時は超高速バタバタになる
		currentWalkFPS = 3 + (int)(13.0f * (currentAbsSpeedX / MARIO_DASH_MAX_SPEED));

		small_mario_walkAnim.FPS = currentWalkFPS;
		small_mario_walkAnim.AnimationUpdateLoop();
	}
	else
	{
		isWalking = false;
		small_mario_walkAnim.currentFrame = 0;
		// 止まっているとき、または空中用のおおよその基準ベース値をスターに渡す用
		currentWalkFPS = isDashing ? 15 : 9;
	}

	//RigidBody update
	if (!StageManager::GetInstance().isBridgeClearing) PhysicsUpdate();
	

	// 物理演算（PhysicsUpdate）が終わった直後に右端のストッパー処理を入れる

	if (position.x + marioWidth > stageRightLimit)
	{
		position.x = stageRightLimit - marioWidth; // 座標を右端限界にクランプ

		// 実際の当たり判定（RigidBody_collider）のX座標も即座に再同期させる
		RigidBody_collider.x = position.x + COLLIDER_OFFSET / 2;

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

	starEffect.Update(isWalking, isActualJumpingPose, currentWalkFPS);
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

	bool isActualJumpingPose = isJumping && (jumpHoldTimer > 0.0f || now_speed_y < 0.0f);

	if (currentForm == MarioForm::SMALL)
	{
		if (starEffect.isActive)
		{
			// スター状態の描画はお任せ
			starEffect.Render(screenX, screenY, isLeft, isActualJumpingPose, isWalking);
		}
		else
		{
			if (isActualJumpingPose)
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
	}
	else if (currentForm == MarioForm::SUPER)
	{
		if (isLeft)	DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, big_mario_waitImage.image, TRUE, TRUE);
		else		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, big_mario_waitImage.image, TRUE, FALSE);
	}
	else if (currentForm == MarioForm::FIRE)
	{
		if (isLeft)	DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, big_mario_fire_waitImage.image, TRUE, TRUE);
		else		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, big_mario_fire_waitImage.image, TRUE, FALSE);
	}


	// DEBUG MODE
	if (map_mode == MODE_DEBUG)
	{
		DrawFormatString(0, 40, GetColor(255, 255, 255), "Mario pos X : %f, Mario pos Y : %f", position.x, position.y);
		DrawFormatString(0, 60, GetColor(255, 255, 255), "now_speed X : %f", now_speed_x);

		//スター状態の残り時間を表示
		if (starEffect.isActive) {
			DrawFormatString(0, 80, GetColor(255, 255, 0), "STAR MODE: %.0f", starEffect.timer);
		}
		else {
			DrawFormatString(0, 80, GetColor(100, 100, 100), "STAR MODE: OFF");
		}

		DrawFormatString(0, 120, GetColor(255, 255, 255), "CTRL + I = マリオが無敵 : %d", debug_is_inivincible);
	}
}

//Jump player
void Mario::Jump(bool playSound)
{
	float currentAbsSpeedX = abs(now_speed_x);

	// 通常歩行の最高速度（4.0f）を超えてダッシュが乗っていたら「ダッシュ大ジャンプ」
	if (currentAbsSpeedX > MARIO_WALK_MAX_SPEED + 0.5f)
	{
		now_speed_y = JUMP_INITIAL_DASH; // -14.0f
	}
	else
	{
		now_speed_y = JUMP_INITIAL_WALK; // -11.5f
	}

	isJumping = true;
	jumpHoldTimer = 0.001f;
	if (playSound)
	{
		SoundManager::GetInstance().PlaySE("Jump_Small");
	}
}

void Mario::AddCoin()
{
	coin++;

	// コイン獲得のSEを再生する処理を追加
	SoundManager::GetInstance().PlaySE("Coin");
}

//starclass(ファイヤマリオも同じように)
//==============================================================
// スター状態の初期化処理(画像とフラグや時間)
//==============================================================
void StarEffect::Init()
{
	isActive = false;
	timer = 0.0f;

	// スター状態は常に色が点滅するため、待機・歩き・ジャンプすべてアニメーションとして読み込む
	// ※画像のファイル名やコマ数などは実際のデータに合わせて修正してください
	waitAnim.InitialAnimation(LoadGraph("data/image/star_mario/starmario_wait.png"), 3, 10);
	walkAnim.InitialAnimation(LoadGraph("data/image/star_mario/starmario_walk.png"), 9, 10);
	jumpAnim.InitialAnimation(LoadGraph("data/image/star_mario/starmario_jump.png"), 3, 10);
}
//==============================================================
// スターを取ったときにスター状態を開始する処理
//==============================================================
void StarEffect::Start()
{
	isActive = true;
	timer = 600.0f; // 例: 60fps環境で10秒間 (60 * 10)
}
//==============================================================
// スター状態の更新処理
// スター状態の残り時間を減らし、アニメーションを更新する
//==============================================================
void StarEffect::Update(bool isWalking, bool isJumping, int walkFPS)
{
	if (isActive)
	{
		timer -= 1.0f;
		if (timer <= 0.0f)
		{
			isActive = false;
		}

		// マリオの現在の状態に合わせて、再生するアニメーションを更新する
		if (isJumping)
		{
			jumpAnim.AnimationUpdateLoop();
		}
		else if (isWalking)
		{
			walkAnim.FPS = walkFPS; // 通常マリオのダッシュ/歩きの速度に合わせる
			walkAnim.AnimationUpdateLoop();
		}
		else
		{
			waitAnim.AnimationUpdateLoop();
		}
	}
}
//==============================================================
// スター状態の描画処理
// マリオの画面座標と向きに合わせてスター状態のアニメーションを描画する
//==============================================================
void StarEffect::Render(int screenX, int screenY, bool isLeft, bool isJumping, bool isWalking)
{
	if (!isActive) return;

	// マリオの現在の状態に合わせて描画するアニメーションを切り替える
	if (isJumping)
	{
		jumpAnim.x = (float)screenX;
		jumpAnim.y = (float)screenY;
		jumpAnim.AnimationRenderCenter(isLeft);
	}
	else if (isWalking)
	{
		walkAnim.x = (float)screenX;
		walkAnim.y = (float)screenY;
		walkAnim.AnimationRenderCenter(isLeft);
	}
	else
	{
		waitAnim.x = (float)screenX;
		waitAnim.y = (float)screenY;
		waitAnim.AnimationRenderCenter(isLeft);
	}


}

void Mario::ChangeToSuper()
{
	if (currentForm == MarioForm::FIRE) return;

	RigidBody_collider = Collider(position.x, position.y, big_mario_waitImage.sizeX, big_mario_waitImage.sizeY);
	RigidBody_collider.width -= COLLIDER_OFFSET;
	RigidBody_collider.x += COLLIDER_OFFSET / 2; // shift right so it's centered
	position.y -= (float)(big_mario_waitImage.sizeY / 2.0f);
	currentForm = MarioForm::SUPER;
}

void Mario::ChangeToFire()
{
	//Change Collider
	RigidBody_collider = Collider(position.x, position.y, big_mario_waitImage.sizeX, big_mario_waitImage.sizeY);
	RigidBody_collider.width -= COLLIDER_OFFSET;
	RigidBody_collider.x += COLLIDER_OFFSET / 2; // shift right so it's centered
	/*position.y -= (float)(big_mario_waitImage.sizeY / 2.0f);*/

	currentForm = MarioForm::FIRE;
}

void Mario::StartCutsceneWalk(float targetX, float speed)
{
	currentState = MarioState::CUTSCENE;
	cutsceneTargetX = targetX;
	cutsceneSpeed = speed;
}

void Mario::WalkTo(float targetX, float speed)
{
	if (position.x < targetX)
	{
		isLeft = false;
		position.x += speed;
		if (position.x > targetX) position.x = targetX; // don't overshoot
	}
	else if (position.x > targetX)
	{
		isLeft = true;
		position.x -= speed;
		if (position.x < targetX) position.x = targetX;
	}

	isWalking = (position.x != targetX);

	if (isWalking)
	{
		small_mario_walkAnim.FPS = 9;
		small_mario_walkAnim.AnimationUpdateLoop();
	}
	else
	{
		small_mario_walkAnim.currentFrame = 0;
	}
}