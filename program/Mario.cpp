#include "Main.h"
#include "Game.h"
#include "Mario.h"
#include "Debug.h"
#include "Camera.h"
#include "Sound.h"

extern Camera MainCamera;

// Camera.cpp との互換性を保つためのグローバル変数
// ※Camera.cpp を変更せずに参照できるようにここに定義しています
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

	if (!collider.intersects(block.collider)) return;

	float overlapLeft = (collider.x + collider.width) - block.collider.x;
	float overlapRight = (block.collider.x + block.collider.width) - collider.x;
	float overlapTop = (collider.y + collider.height) - block.collider.y;
	float overlapBottom = (block.collider.y + block.collider.height) - collider.y;

	float minX = min(overlapLeft, overlapRight);
	float minY = min(overlapTop, overlapBottom);

	const float bias = 0.0f;

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

void Mario::Warping(float pipeY)
{
	currentState = MarioState::WARPING;
	warpTimer = 0.0f;

	now_speed_x = 0.0f;
	now_speed_y = 0.0f;

	position.y = pipeY;
}

// 敵に接触したときに呼び出される死亡開始関数
void Mario::ToDeadState()
{
	if (currentState == MarioState::DEAD) return;

	// スモールマリオの時は即死
	if (currentForm == MarioForm::SMALL)
	{
		currentState = MarioState::DEAD;
		warpTimer = 0.0f; // 死亡後の経過時間タイマーとして流用

		now_speed_x = 0.0f;
		now_speed_y = 0.0f;

		// 死亡した瞬間、当たり判定を完全に無くしてステージや敵をすり抜けさせる
		collider.x = -9999.0f;
		collider.y = -9999.0f;
		collider.width = 0;
		collider.height = 0;

		// 死亡SEを鳴らす（登録されているSE名に合わせてください）
		SoundManager::GetInstance().PlaySE("Mario_Dead");
	}
	else
	{
		// 将来的にスーパーマリオなどの場合は、ここにFormをSMALLに落とす処理を記述
	}
}

void Mario::Init()
{
	// マリオの待機画像を読み込み、初期位置を設定
	small_mario_waitImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_wait.png"));

	// マリオのジャンプ画像を読み込み
	small_mario_jumpImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_jump.png"));

	// 死亡時の画像を読み込み
	small_mario_deadImage.InitialImageAndSize(LoadGraph("data/image/mario/small_mario_dead.png"));

	// 歩きアニメーションの初期化（画像パス、コマ数:3、初期再生速度FPS:10）
	small_mario_walkAnim.InitialAnimation(LoadGraph("data/image/mario/small_mario_walk.png"), 3, 10);

	// 初期状態はスモールマリオ
	currentForm = MarioForm::SMALL; 

	position.Set(165.0f, 700.0f); //772
	isLeft = false; // 最初は右向き

	// 後入力優先用変数の初期化
	prevKeyA = false;
	prevKeyD = false;
	preferLeftInput = false;

	now_speed_x = 0.0f; // 最初は静止している
	now_speed_y = 0.0f;//new added for warp pipe
	currentState = MarioState::NORMAL;
	warpTimer = 0.0f;
	collider = Collider(position.x, position.y, small_mario_waitImage.sizeX, small_mario_waitImage.sizeY); // Collider(コライダーの初期化)
}

void Mario::Update()
{
	// ----------------------------------------------------
	// 死亡状態（DEAD）の更新処理
	// ----------------------------------------------------
	if (currentState == MarioState::DEAD)
	{
		warpTimer += 0.016f; // フレーム時間の加算 (60fps目安)

		// 1. 最初の一瞬（0.5秒間）はその場で完全にフリーズ（硬直）
		if (warpTimer >= 0.5f)
		{
			// フリーズが解けた最初の1フレームだけ、上方向へポコッと跳ね上げる
			if (now_speed_y == 0.0f && (warpTimer - 0.016f) < 0.5f)
			{
				now_speed_y = -12.0f; // 上向きの初速
			}

			// 2. 死亡時専用のカスタム重力演算（PhysicsUpdateを通さないので床を貫通する）
			now_speed_y += 0.5f; // デッド時用の緩やかな重力
			position.y += now_speed_y;
		}

		// 画像位置の更新
		small_mario_deadImage.pos = position;

		// デバッグ画面用の座標更新
		int mScrX = (int)(position.x - MainCamera.pos.x);
		int mScrY = (int)(position.y - MainCamera.pos.y);
		small_mario_debug_x1 = mScrX;
		small_mario_debug_y1 = mScrY;
		small_mario_debug_x2 = mScrX + small_mario_deadImage.sizeX;
		small_mario_debug_y2 = mScrY + small_mario_deadImage.sizeY;

		return; // 死亡時は以降の操作入力や物理演算をすべてスキップして終了
	}

	// ----------------------------------------------------
	// 土管ワープ状態（WARPING）の更新処理
	// ----------------------------------------------------
	if (currentState == MarioState::WARPING)
	{
		warpTimer += 0.016f; // Standard frame step speed calculation

		// Slowly descend Mario down past the threshold grid block boundaries (1.5px frame scale)
		position.y += 1.5f;

		small_mario_waitImage.pos = position;
		collider.x = position.x;
		collider.y = position.y;

		// Frame debug layout configuration parameters
		int mScrX = (int)(position.x - MainCamera.pos.x);
		int mScrY = (int)(position.y - MainCamera.pos.y);
		mario_centerX = mScrX + (small_mario_waitImage.sizeX / 2);
		small_mario_debug_x1 = mScrX;
		small_mario_debug_y1 = mScrY;
		small_mario_debug_x2 = mScrX + small_mario_waitImage.sizeX;
		small_mario_debug_y2 = mScrY + small_mario_waitImage.sizeY;
		// If time finishes, teleport or transition player location
		if (warpTimer >= WARP_DURATION)
		{
			currentState = MarioState::NORMAL;

			// Trigger Underworld level load or coordinate placement modifications here:
			//position.Set(UnderWorldSpawnX, UnderWorldSpawnY);
		}

		return; // Stop processing and drop loop cycles early. Skips normal controls.
	}

	// ----------------------------------------------------
	// 通常状態（NORMAL）の更新処理
	// ----------------------------------------------------
	small_mario_waitImage.pos = position;

	//Collider update
	collider.x = position.x;
	collider.y = position.y;
	
	// 今この瞬間にA/Dキーが押されているか
	bool currKeyA = CheckHitKey(KEY_INPUT_A);
	bool currKeyD = CheckHitKey(KEY_INPUT_D);
	bool isDashing = CheckHitKey(KEY_INPUT_LSHIFT);

	// ----------------------------------------------------
	// 後入力優先の方向決定ロジック
	// ----------------------------------------------------
	// 前フレームでは押されておらず、今フレームで押された方を優先する
	if (currKeyA && !prevKeyA) preferLeftInput = true;
	if (currKeyD && !prevKeyD) preferLeftInput = false;

	// 両押しから片方離した場合、残っている方のキーを優先方向に直す
	if (currKeyA && !currKeyD) preferLeftInput = true;
	if (currKeyD && !currKeyA) preferLeftInput = false;

	// 現在のフレームの状態を次回のために保存
	prevKeyA = currKeyA;
	prevKeyD = currKeyD;

	// 少なくともどちらかのキーが押されているか
	bool moveKeyPressed = currKeyA || currKeyD;

	// ----------------------------------------------------
	// 外部参照用(Camera.cpp等)の移動速度設定
	// ----------------------------------------------------
	if (moveKeyPressed)
	{
		float currentMax;
		// ダッシュ中かどうかで最高速度を変える
		if (isDashing)
		{
			currentMax = MARIO_DASH_MAX_SPEED;
		}
		else
		{
			currentMax = MARIO_WALK_MAX_SPEED;
		}

		// 優先されている向きに応じて速度のプラス・マイナスを決める
		if (preferLeftInput)
		{
			marioSpeed = -currentMax;
		}
		else
		{
			marioSpeed = currentMax;
		}
	}
	else
	{
		marioSpeed = MARIO_WALK_MAX_SPEED; // 慣性滑り時のデフォルト
	}


	// ----------------------------------------------------
	// 速度（now_speed_x）と向きの計算
	// ----------------------------------------------------
	if (moveKeyPressed)
	{
		if (preferLeftInput) // 左移動が優先されている
		{
			// ジャンプ中でない時だけ向きを変える
			if (!isJumping) isLeft = true;

			if (now_speed_x > 0.0f)
			{
				now_speed_x -= MARIO_DECEL_TURN; // 右に動いていたら急ブレーキ
			}
			else
			{
				now_speed_x -= MARIO_ACCEL;      // 通常加速
			}
		}
		else // 右移動が優先されている
		{
			// ジャンプ中でない時だけ向きを変える
			if (!isJumping) isLeft = false;

			if (now_speed_x < 0.0f)
			{
				now_speed_x += MARIO_DECEL_TURN; // 左に動いていたら急ブレーキ
			}
			else
			{
				now_speed_x += MARIO_ACCEL;      // 通常加速
			}
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
	float MarioMoovMaxSpeed;
	if (isDashing)
	{
		MarioMoovMaxSpeed = MARIO_DASH_MAX_SPEED;
	}
	else
	{
		MarioMoovMaxSpeed = MARIO_WALK_MAX_SPEED;
	}

	if (now_speed_x > MarioMoovMaxSpeed)
	{
		now_speed_x = MarioMoovMaxSpeed;
	}
	if (now_speed_x < -MarioMoovMaxSpeed)
	{
		now_speed_x = -MarioMoovMaxSpeed;
	}


	// Dキーの押し下げに関係なく、マリオが画面中央を越えたらカメラを動かすように外に出しました
	float marioWorldCenterX = position.x + (small_mario_waitImage.sizeX / 2.0f);
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
	float marioWidth = small_mario_waitImage.sizeX; // マリオの画像の幅を計算
	float marioHeight = small_mario_waitImage.sizeY;// マリオの画像の高さを計算
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

	small_mario_debug_x1 = mario_screenX;
	small_mario_debug_y1 = mario_screenY;
	small_mario_debug_x2 = mario_screenX + marioWidth;
	small_mario_debug_y2 = mario_screenY + marioHeight;

	// first press — initial jump
	if (CheckHitKey(KEY_INPUT_SPACE) && !isJumping)
	{
		now_speed_y = JUMP_INITIAL;  // shoot up
		isJumping = true;
		jumpHoldTimer = 0.0f;

		// ジャンプした瞬間にジャンプ音を鳴らす
		SoundManager::GetInstance().PlaySE("Jump_Small");
	}

	// hold to go higher
	if (CheckHitKey(KEY_INPUT_SPACE) && isJumping && jumpHoldTimer < JUMP_HOLD_MAX)
	{
		now_speed_y += JUMP_HOLD_FORCE;  // extra upward push
		jumpHoldTimer += 0.016f;         // add time (~1 frame at 60fps)
	}

	// 歩きアニメーションのループ更新処理（RigidBodyの物理演算の直前に判定）
	if (moveKeyPressed && !isJumping)
	{
		isWalking = true;

		// ダッシュ中か通常歩行中かでシャカシャカ度（アニメーション速度）を切り替える
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			small_mario_walkAnim.FPS = 15; // ダッシュ時はアニメーションを速くする
		}
		else
		{
			small_mario_walkAnim.FPS = 9;  // 通常時はトコトコ動かす
		}

		small_mario_walkAnim.AnimationUpdateLoop(); // アニメーションのコマを進める
	}
	else
	{
		// 立ち止まった、またはジャンプした時はアニメーションをオフにして最初のコマに戻す
		isWalking = false;
		small_mario_walkAnim.currentFrame = 0;
	}

	//RigidBody update
	PhysicsUpdate();
}

void Mario::Render()
{
	// カメラの座標に合わせてマリオの描画位置（スクリーン座標）を計算
	int screenX = (int)(small_mario_waitImage.pos.x - MainCamera.pos.x);
	int screenY = (int)(small_mario_waitImage.pos.y - MainCamera.pos.y);
	
	// 死亡時は最優先でデッド画像を描画して即リターン
	if (currentState == MarioState::DEAD)
	{
		DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_deadImage.image, TRUE, FALSE);
		return;
	}

	// ジャンプ中 ＞ 歩行中 ＞ 待機中 の優先順位で描画を切り替える
	if (isJumping)
	{
		// 【修正】jumpImage.image を使用して描画
		if (isLeft)
		{
			// 左向きジャンプ (左右反転)
			DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_jumpImage.image, TRUE, TRUE);
		}
		else
		{
			// 右向きジャンプ (反転なし)
			DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_jumpImage.image, TRUE, FALSE);
		}
	}
	else if (isWalking)
	{
		// 歩き状態のアニメーション描画
		small_mario_walkAnim.x = (float)screenX;
		small_mario_walkAnim.y = (float)screenY;

		// 自身のAnimationクラスに備わっている関数にisLeft（左を向いているか）をそのまま渡す
		// これにより左移動時は自動で左右反転されて描画されます
		small_mario_walkAnim.AnimationRenderCenter(isLeft);
	}
	else
	{
		// 待機状態（止まっている時）は向きに応じて反転させて描画
		if (isLeft)
		{
			// 左向き (左右反転)
			DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_waitImage.image, TRUE, TRUE);
		}
		else
		{
			// 右向き (反転なし)
			DrawRotaGraph2(screenX, screenY, 0, 0, 1.0f, 0.0, small_mario_waitImage.image, TRUE, FALSE);
		}
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