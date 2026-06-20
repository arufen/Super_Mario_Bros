#include "Main.h"
#include "Game.h"
#include "Mario.h"
#include "Debug.h"
#include "Camera.h"
#include "StageManager.h"
#include "Sound.h"

// 外部のカメラ情報を参照
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

	// Trigger（接触判定のみのオブジェクト）の処理
	if (block.isTrigger)
	{
		// コインの獲得
		Coin* coin = dynamic_cast<Coin*>(&block);
		if (coin != nullptr)
		{
			AddCoin();
			coin->active = false;
		}

		// アイテムの獲得
		CollectableItem* item = dynamic_cast<CollectableItem*>(&block);
		if (item != nullptr && item->canCollect)
		{
			item->CollectItem(*this);
			MainScore.AddScore(1000);
		}

		block.OnHitSide(*this);
		return;
	}

	// 各方向のめり込み量（overlap）を計算
	float overlapLeft = (RigidBody_collider.x + RigidBody_collider.width) - block.collider.x;
	float overlapRight = (block.collider.x + block.collider.width) - RigidBody_collider.x;
	float overlapTop = (RigidBody_collider.y + RigidBody_collider.height) - block.collider.y;
	float overlapBottom = (block.collider.y + block.collider.height) - RigidBody_collider.y;

	float minX = min(overlapLeft, overlapRight);
	float minY = min(overlapTop, overlapBottom);

	// 空中での「壁擦り（壁への押し付け）」を検出するロジック
	bool isWallScraping = false;
	if (isJumping)
	{
		// ブロックがマリオの右側にあるか、左側にあるか
		bool isBlockOnRight = (overlapLeft < overlapRight);

		// マリオがその壁の方向にキーを入力している、またはその方向の速度があるか
		bool isPushingWall = false;
		if (isBlockOnRight && (CheckHitKey(KEY_INPUT_D) || now_speed_x > 0.0f)) isPushingWall = true;
		if (!isBlockOnRight && (CheckHitKey(KEY_INPUT_A) || now_speed_x < 0.0f)) isPushingWall = true;

		// 最高点付近（now_speed_yが小さい）で壁に押し付けており、
		// かつ横・縦の重なりが浅い（ブロックの側面の継ぎ目）場合は「壁擦り」とみなす
		if (isPushingWall && minX <= abs(now_speed_x) + 2.0f && overlapTop <= abs(now_speed_y) + 4.0f)
		{
			isWallScraping = true;
		}
	}

	// -------------------------------------------------------------------------
	// 1. 最優先の床（着地）判定判定
	// -------------------------------------------------------------------------
	float maxOverlapTopLimit = max(8.0f, now_speed_y + 4.0f);
	if (!isWallScraping && now_speed_y >= 0.0f && overlapTop < overlapBottom && minX > 2.0f && overlapTop <= maxOverlapTopLimit)
	{
		if (hitVertical) return;

		// 床（着地）処理：めり込みを戻す
		position.y -= overlapTop;
		RigidBody_collider.y -= overlapTop;
		now_speed_y = 0.0f;
		isJumping = false;
		jumpHoldTimer = 0.0f;
		hitVertical = true;

		block.OnHitTop(*this);
		return;
	}

	// -------------------------------------------------------------------------
	// 2. バイアス（優先度）の動的決定（床の継ぎ目引っかかり対策）
	// -------------------------------------------------------------------------
	float bias = 0.5f;
	if (dynamic_cast<Enemy*>(&block) != nullptr)
	{
		bias = 14.0f; // 敵に対しては「理不尽な横死」を防ぐために大きめのバイアスを維持
	}
	else
	{
		if (!isJumping)
		{
			// 地上にいる時は、床の継ぎ目で引っかからないようにバイアスを大きくして床判定（縦）を優先
			bias = 5.0f;
		}
		else
		{
			// 空中（ジャンプ中）でも、落下中（着地寸前）でブロックの上面に触れている場合は、
			// 床の継ぎ目に引っかからないようにバイアスを大きくして床判定（縦）を優先
			if (now_speed_y >= 0.0f && overlapTop < overlapBottom)
			{
				// その場での垂直ジャンプ時（横入力なし・横移動の慣性なし）かつ、横のめり込みがごく浅い場合は、
				// 床判定に吸い込まれないようにバイアスを 0 にして壁判定（横の押し戻し）を最優先にする
				bool hasHorizontalMomentum = (CheckHitKey(KEY_INPUT_A) || CheckHitKey(KEY_INPUT_D) || abs(now_speed_x) > 0.1f);
				if (!hasHorizontalMomentum && minX < 4.0f)
				{
					bias = 0.0f;
				}
				else
				{
					bias = max(5.0f, now_speed_y + 4.0f);
				}
			}
			else
			{
				// 上昇中、または天井付近のときは、頭突きと誤認させないようにバイアスを小さくして壁判定（横）を優先
				bias = 0.2f;
			}
		}
	}

	// -------------------------------------------------------------------------
	// 3. 縦判定（床・天井）か、横判定（壁）かの分岐
	// -------------------------------------------------------------------------
	bool isBrushingCornerWhileRising = (now_speed_y < 0.0f && overlapTop < overlapBottom) || isWallScraping;

	if (minY < minX + bias && !isBrushingCornerWhileRising)
	{
		// -----------------------------------------------------------------
		// 【縦方向の衝突補正】
		// -----------------------------------------------------------------
		if (hitVertical) return;

		if (overlapTop < overlapBottom)
		{
			// 【床への着地補正】
			if (now_speed_y >= 0.0f)
			{
				position.y -= overlapTop;
				RigidBody_collider.y -= overlapTop;
				now_speed_y = 0.0f;
				isJumping = false;
				jumpHoldTimer = 0.0f;
				hitVertical = true;

				block.OnHitTop(*this);
			}
		}
		else
		{
			// 【天井への頭突き補正】完全な真下からの衝突
			position.y += overlapBottom;
			RigidBody_collider.y += overlapBottom;

			now_speed_y = 0.0f;
			jumpHoldTimer = JUMP_HOLD_MAX; // ホールド強制終了

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
					/*switch (questionBlock->itemType)
					{
					case QuestionBlockItem::COIN: AddCoin(); break;
					}*/
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
		// -----------------------------------------------------------------
		// 【横方向の衝突補正】（壁へのめり込み防止・押し戻し）
		// -----------------------------------------------------------------
		// すでに縦判定で位置補正が行われており、かつ重なりがごく僅かなら横補正をスキップしてガタつきを防ぐ
		if (hitVertical && overlapTop <= 4.0f)
		{
			return;
		}

		// 左右のめり込み量を比較して、より浅い方向（めり込んだ側）へ正確に押し戻す
		if (overlapLeft < overlapRight)
		{
			// 左側からの衝突（ブロックがマリオの右にある）：左へ押し戻す
			position.x -= overlapLeft;
			RigidBody_collider.x -= overlapLeft;

			// 右方向への移動速度を持っていたらゼロにする
			if (now_speed_x > 0.0f) now_speed_x = 0.0f;
		}
		else
		{
			// 右側からの衝突（ブロックがマリオの左にある）：右へ押し戻す
			position.x += overlapRight;
			RigidBody_collider.x += overlapRight;

			// 左方向への移動速度を持っていたらゼロにする
			if (now_speed_x < 0.0f) now_speed_x = 0.0f;
		}

		block.OnHitSide(*this);
	}
}

// 土管などでのワープ処理を開始する関数
void Mario::Warping(float targetX, float targetY, WarpDir dir)
{
	currentState = MarioState::WARPING;
	currentWarpDir = dir;
	warpTimer = 0.0f;

	// ワープ中は移動速度をゼロにする
	now_speed_x = 0.0f;
	now_speed_y = 0.0f;

	position.x = targetX;
	position.y = targetY;
}

// ダメージを受けた時の処理
void Mario::TakeDamage()
{
	// スター状態、または無敵時間中はダメージを無効化
	if (starEffect.isActive) return;
	if (invincibleTimer.GetCurrentTimer() > 0.0f) return;

	// 現在の形態に応じて段階を下げるか、死亡状態にする
	if (currentForm == MarioForm::FIRE)
	{
		SetForm(MarioForm::SUPER);
	}
	else if (currentForm == MarioForm::SUPER)
	{
		SetForm(MarioForm::SMALL);
	}
	else
	{
		ToDeadState();
	}

	// ダメージ後、一時的な無敵時間を設定
	invincibleTimer.SetTimer(2.0f);
}

// マリオの形態（チビ、スーパー、ファイア）を変更する関数
void Mario::SetForm(MarioForm form)
{
	MarioForm previousForm = currentForm;
	currentForm = form;

	bool wasSmall = (previousForm == MarioForm::SMALL);
	bool isSmall = (currentForm == MarioForm::SMALL);

	// 大きさが変わる場合（チビからスーパー等）、めり込み防止のため座標と当たり判定（コライダー）の高さを補正
	if (wasSmall && !isSmall)
	{
		position.y -= BLOCK_SIZE; // 大きくなる時は上に押し上げる
		RigidBody_collider.height = big_mario_waitImage.sizeY;
	}
	else if (!wasSmall && isSmall)
	{
		position.y += BLOCK_SIZE; // 小さくなる時は下げる
		RigidBody_collider.height = small_mario_waitImage.sizeY;
	}

	// 形態変化に応じたSEの再生
	if (wasSmall && !isSmall)
	{
		SoundManager::GetInstance().PlaySE("Power_Up");
	}
	else if (previousForm == MarioForm::SUPER && currentForm == MarioForm::FIRE)
	{
		SoundManager::GetInstance().PlaySE("Power_Up");
	}
	else if (!wasSmall && isSmall)
	{
		SoundManager::GetInstance().PlaySE("Warp"); // ダメージダウン時のSE
	}
	else if (previousForm == MarioForm::FIRE && currentForm == MarioForm::SUPER)
	{
		SoundManager::GetInstance().PlaySE("Warp");
	}
}

// 死亡状態への移行処理
void Mario::ToDeadState(bool isFall)
{
	if (currentState == MarioState::DEAD) return;

	// 落下死以外の場合は、無敵状態なら死亡しない
	if (!isFall)
	{
		if (invincibleTimer.GetCurrentTimer() > 0.0f) return;
		if (starEffect.isActive) return;
	}

	// 落下死、またはチビ状態でのダメージ死の処理
	if (isFall || currentForm == MarioForm::SMALL)
	{
		currentState = MarioState::DEAD;
		isFellDown = isFall;
		warpTimer = 0.0f; // 死亡演出用のタイマーとして再利用

		now_speed_x = 0.0f;
		now_speed_y = 0.0f;

		// 死亡時は当たり判定を消滅させる
		RigidBody_collider.width = 0;
		RigidBody_collider.height = 0;

		SoundManager::GetInstance().PlayDeadBGM();
	}
}

// マリオの初期化処理
void Mario::Init()
{
	// 各種画像とアニメーションの読み込み・初期化
	small_mario_waitImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_wait.png"));
	small_mario_jumpImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_jump.png"));
	small_mario_deadImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_dead.png"));
	small_mario_goalpoleImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_goalpole.png"));
	small_mario_walkAnim.InitialAnimation(LoadGraph("data/image/mario/small_mario_walk.png"), 3, 10);
	big_mario_waitImage.InitialImageAndSize(LoadGraph("data/image/big_mario_wait.png"));
	big_mario_fire_waitImage.InitialImageAndSize(LoadGraph("data/image/big_mario_fire_wait.png"));
	

	// 初期ステータスのセットアップ
	currentForm = MarioForm::SMALL;
	position.Set(165.0f, 700.0f);
	isLeft = false;
	isDeadJumped = false;
	isFellDown = false;

	now_speed_x = 0.0f;
	now_speed_y = 0.0f;
	currentState = MarioState::NORMAL;
	warpTimer = 0.0f;

	// 初期当たり判定（コライダー）の設定
	RigidBody_collider = Collider(position.x, position.y, small_mario_waitImage.sizeX, small_mario_waitImage.sizeY);
	RigidBody_collider.width -= COLLIDER_OFFSET;
	RigidBody_collider.x += COLLIDER_OFFSET / 2;

	coin = 0;

	starEffect.Init();
	debug_is_inivincible = false;
}
void Mario::WarpPosition(float targetX, float targetY)
{
	position.Set(targetX, targetY);
	now_speed_x = 0.0f;
	now_speed_y = 0.0f;
	currentState = MarioState::NORMAL;
	warpTimer = 0.0f;

	// 現在の形態（チビか大きいか）に合わせてコライダーの高さを再設定
	if (currentForm == MarioForm::SMALL) {
		RigidBody_collider.height = small_mario_waitImage.sizeY;
	}
	else {
		RigidBody_collider.height = big_mario_waitImage.sizeY;
	}
	RigidBody_collider.x = position.x + COLLIDER_OFFSET / 2;
	RigidBody_collider.y = position.y;
}
// 毎フレームの更新処理（状態ごとの分岐と物理演算・入力処理）
void Mario::Update()
{
	// デバッグ用無敵モード切り替え (LCTRL + I)
	if (map_mode == MODE_DEBUG && CheckHitKey(KEY_INPUT_LCONTROL) && PushHitKey(KEY_INPUT_I))
	{
		debug_is_inivincible = !debug_is_inivincible ? true : false;
	}
	if (debug_is_inivincible) invincibleTimer.SetTimer(1.0f);

	invincibleTimer.Update();

	// ----------------------------------------------------
	// 死亡状態（DEAD）の更新処理
	// ----------------------------------------------------
	if (currentState == MarioState::DEAD)
	{
		warpTimer += 0.016f;

		// 穴に落ちた場合は描画やジャンプ演出を行わない
		if (isFellDown)
		{
			now_speed_x = 0.0f;
			now_speed_y = 0.0f;

			small_mario_debug_x1 = -9999;
			small_mario_debug_y1 = -9999;
			small_mario_debug_x2 = -9999;
			small_mario_debug_y2 = -9999;
			return;
		}

		// デスジャンプ演出（少し停止した後に飛び上がる）
		if (warpTimer >= 0.5f)
		{
			if (!isDeadJumped)
			{
				now_speed_y = -12.0f;
				isDeadJumped = true;
			}

			now_speed_y += 0.5f; // 重力
			position.y += now_speed_y;
		}
		else
		{
			now_speed_y = 0.0f;
		}

		small_mario_deadImage.pos = position;

		// デバッグ描画用座標の更新
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

		PhysicsUpdate(); // 重力などの物理挙動を適用

		// カットシーン中もカメラを右方向へ追従させる
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
	// ゴール演出状態（GOAL）の更新処理
	// ----------------------------------------------------
	if (currentState == MarioState::GOAL)
	{
		if (goalPhase == 0) // ポールを滑り降りるフェーズ
		{
			now_speed_x = 0.0f;
			now_speed_y = 0.0f;
			isLeft = false;
			position.y += 3.0f; // 滑り降りる速度

			float marioBottom = position.y + small_mario_goalpoleImage.sizeY;
			if (marioBottom >= goalFloorY)
			{
				position.y = goalFloorY - small_mario_goalpoleImage.sizeY;
				goalPhase = 1;
				warpTimer = 0.0f;
			}
		}
		else if (goalPhase == 1) // ポール着地後、旗のSE完了を待つフェーズ
		{
			now_speed_x = 0.0f;
			now_speed_y = 0.0f;
			if (SoundManager::GetInstance().IsPlayingSE("Flag_Pole"))
			{
				isLeft = false;
			}
			else
			{
				// SE終了後、ポールの反対側へ反転
				isLeft = true;
				position.x = goalPoleX - 5.0f;

				warpTimer += 0.016f;
				if (warpTimer >= 0.5f)
				{
					goalPhase = 2; // 城へ向かって歩き出すフェーズへ
				}
			}
		}
		else if (goalPhase == 2) // 城へ歩くフェーズ
		{
			isLeft = false;
			now_speed_x = 2.0f;

			PhysicsUpdate();

			small_mario_walkAnim.FPS = 8;
			small_mario_walkAnim.AnimationUpdateLoop();

			if (position.x >= goalCastleX)
			{
				position.x = goalCastleX;
				now_speed_x = 0.0f;
				goalPhase = 3; // 城に到着（演出終了）
			}
		}
		else if (goalPhase == 3)
		{
			now_speed_x = 0.0f;
			now_speed_y = 0.0f;
			isWalking = false;
		}

		RigidBody_collider.x = position.x + COLLIDER_OFFSET / 2;
		RigidBody_collider.y = position.y;

		// ゴール演出中もカメラ追従
		if (goalPhase == 2 && MainCamera.pos.y < 960)
		{
			float marioWorldCenterX = position.x + (small_mario_waitImage.sizeX / 2.0f);
			if (marioWorldCenterX >= MainCamera.pos.x + (SCREEN_W / 2))
			{
				MainCamera.pos.x += now_speed_x;
			}
		}

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

		// ワープ演出中の座標スライド移動
		if (warpTimer < WARP_DURATION)
		{
			if (currentWarpDir == WarpDir::DOWN)       position.y += 1.5f;
			else if (currentWarpDir == WarpDir::RIGHT) position.x += 1.5f;
			else if (currentWarpDir == WarpDir::UP)    position.y -= 1.5f;
		}

		now_speed_x = 0.0f;
		now_speed_y = 0.0f;

		//small_mario_waitImage.pos = position;
		if (currentForm == MarioForm::SUPER)
		{
			big_mario_waitImage.pos = position;
		}
		else if (currentForm == MarioForm::FIRE)
		{
			big_mario_fire_waitImage.pos = position;
		}
		else // Default to SMALL
		{
			small_mario_waitImage.pos = position;
		}

		int scrX1 = (int)(RigidBody_collider.x - MainCamera.pos.x);
		int scrY1 = (int)(RigidBody_collider.y - MainCamera.pos.y);
		small_mario_debug_x1 = scrX1;
		small_mario_debug_y1 = scrY1;
		small_mario_debug_x2 = scrX1 + (int)RigidBody_collider.width;
		small_mario_debug_y2 = scrY1 + (int)RigidBody_collider.height;
		mario_centerX = scrX1 + ((int)RigidBody_collider.width / 2);

		// ワープ演出完了後のエリア遷移処理
		if (warpTimer >= WARP_DURATION)
		{
			if (SoundManager::GetInstance().IsPlayingBGM("HurryAction")) return;

			if (currentWarpDir == WarpDir::DOWN)
			{
				StageManager::GetInstance().TransferWorldZone(WorldZone::UNDERWORLD);
			}
			else if (currentWarpDir == WarpDir::RIGHT)
			{
				StageManager::GetInstance().TransferWorldZone(WorldZone::OVERWORLD);
			}

			currentState = MarioState::NORMAL;

		}
		return;
	}

	// ----------------------------------------------------
	// 通常状態（NORMAL）の更新処理
	// ----------------------------------------------------

	// デバッグ用スター状態切り替え (Pキー)
	if (map_mode == MODE_DEBUG && CheckHitKey(KEY_INPUT_P))
	{
		starEffect.Start();
	}

	// 落下死（デッドライン越え）の判定
	float currentDeadLineY = (StageManager::GetInstance().GetWorldZone() == WorldZone::OVERWORLD)
		? DEAD_LINE_OVERWORLD
		: DEAD_LINE_UNDERWORLD;

	if (position.y > currentDeadLineY)
	{
		ToDeadState(true); // isFall = true
		return;
	}

	small_mario_waitImage.pos = position;

	RigidBody_collider.x = position.x + COLLIDER_OFFSET / 2;
	RigidBody_collider.y = position.y;

	// 入力状態の取得
	bool currKeyA = CheckHitKey(KEY_INPUT_A);
	bool currKeyD = CheckHitKey(KEY_INPUT_D);
	bool isDashing = CheckHitKey(KEY_INPUT_LSHIFT);
	bool currKeySpace = CheckHitKey(KEY_INPUT_SPACE);

	// 同時押し対策：後から押されたキーの方向を優先するロジック
	if (currKeyA && !prevKeyA) preferLeftInput = true;
	if (currKeyD && !prevKeyD) preferLeftInput = false;
	if (currKeyA && !currKeyD) preferLeftInput = true;
	if (currKeyD && !currKeyA) preferLeftInput = false;

	prevKeyA = currKeyA;
	prevKeyD = currKeyD;

	bool moveKeyPressed = currKeyA || currKeyD;

	// 空中・地上に応じた加速度、摩擦、ターン減速度の設定
	float activeAccel = isJumping ? MARIO_AIR_ACCEL : MARIO_ACCEL;
	float activeFriction = isJumping ? MARIO_AIR_FRICTION : MARIO_FRICTION;
	float activeTurn = isJumping ? MARIO_AIR_ACCEL : MARIO_DECEL_TURN;

	// 最高速度の設定（ダッシュキー有無）
	float maxSpeed = isDashing ? MARIO_DASH_MAX_SPEED : MARIO_WALK_MAX_SPEED;

	// 左右の移動処理
	if (moveKeyPressed)
	{
		if (preferLeftInput)
		{
			if (!isJumping) isLeft = true;

			// 逆方向に移動中の場合はターン用減速度（ブレーキ）を適用
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
		// 入力なし時は摩擦で徐々に停止させる
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

	// 速度の制限（最高速度キャップ）
	if (now_speed_x > maxSpeed)  now_speed_x = maxSpeed;
	if (now_speed_x < -maxSpeed) now_speed_x = -maxSpeed;

	marioSpeed = now_speed_x;

	// カメラのスクロール追従処理（画面中央を越えたらカメラが右へ動く）
	if (MainCamera.pos.y < 960)
	{
		float marioWorldCenterX = position.x + (small_mario_waitImage.sizeX / 2.0f);
		if (marioWorldCenterX >= MainCamera.pos.x + (SCREEN_W / 2))
		{
			MainCamera.pos.x = marioWorldCenterX - (SCREEN_W / 2);
		}
	}

	// 画面左端に戻れないようにする壁の判定
	if (position.x < MainCamera.pos.x)
	{
		position.x = MainCamera.pos.x;
	}

	float marioWidth = small_mario_waitImage.sizeX;
	float stageRightLimit = 12480.0f + SCREEN_W; // ステージ右端の限界値

	// 落下開始の判定（床から離れた時）
	if (!isJumping && now_speed_y > 0.0f)
	{
		isJumping = true;
	}

	// ジャンプ開始処理
	if (currKeySpace && !prevKeySpace && !isJumping && now_speed_y == 0.0f)
	{
		Jump();
	}

	// ジャンプ中のボタン長押しによる滞空処理（押し続けると高く飛べる）
	if (currKeySpace && isJumping && jumpHoldTimer > 0.0f && jumpHoldTimer < JUMP_HOLD_MAX)
	{
		now_speed_y += JUMP_HOLD_FORCE;
		jumpHoldTimer += 0.016f;
	}

	prevKeySpace = currKeySpace;

	// 歩行アニメーションのフレーム速度計算
	float currentAbsSpeedX = abs(now_speed_x);
	int currentWalkFPS = 0;

	// 実際のジャンプ状態（上昇中、またはジャンプホールド中）かどうかの判定
	bool isActualJumpingPose = isJumping && (jumpHoldTimer > 0.0f || now_speed_y < 0.0f);

	if (moveKeyPressed && !isActualJumpingPose)
	{
		isWalking = true;
		// 速度に応じてアニメーション速度を変える
		currentWalkFPS = 3 + (int)(13.0f * (currentAbsSpeedX / MARIO_DASH_MAX_SPEED));

		small_mario_walkAnim.FPS = currentWalkFPS;
		small_mario_walkAnim.AnimationUpdateLoop();
	}
	else
	{
		isWalking = false;
		small_mario_walkAnim.currentFrame = 0; // 静止時はアニメーションリセット
		currentWalkFPS = isDashing ? 15 : 9;
	}

	// 橋崩壊イベント中でなければ物理更新
	if (!StageManager::GetInstance().isBridgeClearing) PhysicsUpdate();

	// ステージ右端の壁の判定
	if (position.x + marioWidth > stageRightLimit)
	{
		position.x = stageRightLimit - marioWidth;
		RigidBody_collider.x = position.x + COLLIDER_OFFSET / 2;

		if (now_speed_x > 0.0f) now_speed_x = 0.0f;
	}

	// デバッグ描画用の座標更新
	int scrX1 = (int)(RigidBody_collider.x - MainCamera.pos.x);
	int scrY1 = (int)(RigidBody_collider.y - MainCamera.pos.y);

	small_mario_debug_x1 = scrX1;
	small_mario_debug_y1 = scrY1;
	small_mario_debug_x2 = scrX1 + (int)RigidBody_collider.width;
	small_mario_debug_y2 = scrY1 + (int)RigidBody_collider.height;

	mario_centerX = scrX1 + ((int)RigidBody_collider.width / 2);

	// スーパースター状態の更新
	starEffect.Update(isWalking, isActualJumpingPose, currentWalkFPS);
}

// 描画処理
void Mario::Render()
{
	// 画面上での描画座標の計算
	int screenX = (int)(position.x - MainCamera.pos.x);
	int screenY = (int)(position.y - MainCamera.pos.y);

	// 死亡時の描画
	if (currentState == MarioState::DEAD)
	{
		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_deadImage.image, TRUE, FALSE);
		return;
	}

	// 土管ワープ中の描画（常に待機画像）
	if (currentState == MarioState::WARPING)
	{
		if (isLeft)	DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_waitImage.image, TRUE, TRUE);
		else		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_waitImage.image, TRUE, FALSE);
		return;
	}

	// ゴール演出中の描画
	if (currentState == MarioState::GOAL)
	{
		if (goalPhase == 0)      DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_goalpoleImage.image, TRUE, FALSE);
		else if (goalPhase == 1) DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_goalpoleImage.image, TRUE, isLeft);
		else if (goalPhase == 2)
		{
			small_mario_walkAnim.x = (float)screenX;
			small_mario_walkAnim.y = (float)screenY;
			small_mario_walkAnim.AnimationRenderCenter(isLeft);
		}
		return;
	}

	bool isActualJumpingPose = isJumping && (jumpHoldTimer > 0.0f || now_speed_y < 0.0f);

	// 現在の形態（チビ・スーパー・ファイア）ごとの画像描画
	if (currentForm == MarioForm::SMALL)
	{
		if (starEffect.isActive)
		{
			// 無敵スター状態の描画
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

	// デバッグ用情報の画面表示
	if (map_mode == MODE_DEBUG)
	{
		DrawFormatString(0, 40, GetColor(255, 255, 255), "Mario pos X : %f, Mario pos Y : %f", position.x, position.y);
		DrawFormatString(0, 60, GetColor(255, 255, 255), "now_speed X : %f", now_speed_x);

		if (starEffect.isActive) DrawFormatString(0, 80, GetColor(255, 255, 0), "STAR MODE: %.0f", starEffect.timer);
		else                     DrawFormatString(0, 80, GetColor(100, 100, 100), "STAR MODE: OFF");

		DrawFormatString(0, 120, GetColor(255, 255, 255), "CTRL + I = マリオが無敵 : %d", debug_is_inivincible);
	}
}

// ジャンプ処理
void Mario::Jump(bool playSound)
{
	float currentAbsSpeedX = abs(now_speed_x);

	// ダッシュ時と歩行時でジャンプの初速を切り替える
	if (currentAbsSpeedX > MARIO_WALK_MAX_SPEED + 0.5f)	now_speed_y = JUMP_INITIAL_DASH;
	else												now_speed_y = JUMP_INITIAL_WALK;

	isJumping = true;
	jumpHoldTimer = 0.001f; // ジャンプホールド開始
	if (playSound) SoundManager::GetInstance().PlaySE("Jump_Small");
}

// コイン加算処理
void Mario::AddCoin()
{
	coin++;
	MainScore.AddScore(50);
	SoundManager::GetInstance().PlaySE("Coin");
}

// ==============================================================
// StarEffect クラスの実装 (スーパースター取得時の無敵エフェクト)
// ==============================================================
void StarEffect::Init()
{
	isActive = false;
	timer = 0.0f;

	// スター状態用アニメーション画像の読み込み
	waitAnim.InitialAnimation(LoadGraph("data/image/star_mario/starmario_wait.png"), 3, 10);
	walkAnim.InitialAnimation(LoadGraph("data/image/star_mario/starmario_walk.png"), 9, 10);
	jumpAnim.InitialAnimation(LoadGraph("data/image/star_mario/starmario_jump.png"), 3, 10);
}

void StarEffect::Start()
{
	isActive = true;
	timer = 600.0f; // 約10秒間 (60FPS想定で 600フレーム)
}

void StarEffect::Update(bool isWalking, bool isJumping, int walkFPS)
{
	if (isActive)
	{
		timer -= 1.0f;
		if (timer <= 0.0f) isActive = false; // 時間切れで効果終了

		// 色変化アニメーションの更新
		waitAnim.AnimationUpdateLoop();
		walkAnim.FPS = 15;
		walkAnim.AnimationUpdateLoop();
		jumpAnim.AnimationUpdateLoop();
	}
}

void StarEffect::Render(int screenX, int screenY, bool isLeft, bool isJumping, bool isWalking)
{
	if (!isActive) return;

	// アクションの状態に応じてスター用画像を切り替えて描画
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

// カットシーン用：指定座標への自動歩行開始
void Mario::StartCutsceneWalk(float targetX, float speed)
{
	currentState = MarioState::CUTSCENE;
	cutsceneTargetX = targetX;
	cutsceneSpeed = speed;
}

// 内部用：指定座標に向けて自動で移動・アニメーションさせる
void Mario::WalkTo(float targetX, float speed)
{
	if (position.x < targetX)
	{
		isLeft = false;
		position.x += speed;
		if (position.x > targetX) position.x = targetX;
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

// ゴール時の専用演出の初期化処理
void Mario::StartGoalCutscene(float poleX, float castleX, float floorY)
{
	currentState = MarioState::GOAL;
	goalPhase = 0;
	goalPoleX = poleX;
	goalCastleX = castleX;
	goalFloorY = floorY;

	now_speed_x = 0.0f;
	now_speed_y = 0.0f;

	// ポールに掴まる座標への補正
	position.x = poleX - 45.0f;
}