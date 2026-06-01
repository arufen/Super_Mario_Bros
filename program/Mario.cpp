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
// NOTE: 他箇所で同名のシンボルが定義されている可能性があるため
// 定数名を一意にする（C2377 対策）。
const float MARIO_ACCEL = 0.4f;		// 1フレームごとの加速度（増やすとキレが良くなる）
const float MARIO_WALK_MAX_SPEED = 4.5f;	// 歩き状態の最高速度（これ以上速くならない）
const float MARIO_DASH_MAX_SPEED = 8.0f;   // ダッシュ状態の最高速度（これ以上速くならない）
const float MARIO_FRICTION = 0.3f;	// キーを離したときの摩擦・ブレーキ（減らすとよく滑る）
const float MARIO_DECEL_TURN = 0.8f;  // 逆キーを入れたときの急ブレーキの強さ

void Mario::Init()
{
	// 【修正】walkAnimation を正しく初期化（画像、コマ数:3, FPS:15）
	walkAnimation.InitialAnimation(LoadGraph("data/mario/mario_walk_01.png"), 3, 15);

	marioImage.pos.Set(165.0f, 772.0f);

	isLeft = false; // 最初は右向き
	now_speed_x = 0.0f; // 最初は静止している
}

void Mario::Update()
{
	// LSHIFT + 方向キー の組み合わせは単独の方向キー判定より先に評価する
	if (CheckHitKey(KEY_INPUT_LSHIFT) && CheckHitKey(KEY_INPUT_D)) {
		marioSpeed = MARIO_DASH_MAX_SPEED; // ダッシュの最高速度（右向き）
	}
	else if (CheckHitKey(KEY_INPUT_D)) {
		marioSpeed = MARIO_WALK_MAX_SPEED; // 歩きの最高速度（右向き）
		// 【修正】ここでは更新せず、Updateの最後で一括して walkAnimation の更新を行います
	}
	else if (CheckHitKey(KEY_INPUT_LSHIFT) && CheckHitKey(KEY_INPUT_A)) {
		marioSpeed = -MARIO_DASH_MAX_SPEED; // ダッシュの最高速度（左向き）
	}
	else if (CheckHitKey(KEY_INPUT_A)) {
		marioSpeed = -MARIO_WALK_MAX_SPEED; // 歩きの最高速度（左向き）
		// 【修正】ここでは更新せず、Updateの最後で一括して walkAnimation の更新を行います
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

	// 計算した速度を「実際の座標」に足し算する
	marioImage.pos.x += now_speed_x;

	// Dキーの押し下げに関係なく、マリオが画面中央を越えたらカメラを動かすように外に出しました
	float marioWorldCenterX = marioImage.pos.x + (marioImage.sizeX / 2.0f);
	if (marioWorldCenterX >= MainCamera.pos.x + (SCREEN_W / 2))
	{
		MainCamera.pos.x = marioWorldCenterX - (SCREEN_W / 2);
	}

	// カメラの移動可能範囲を制限 (0 ～ 12480)
	if (MainCamera.pos.x < 0) MainCamera.pos.x = 0;
	if (MainCamera.pos.x > 12480) MainCamera.pos.x = 12480;

	// マリオがカメラの左端より外に出ないようにする
	if (marioImage.pos.x < MainCamera.pos.x)
	{
		marioImage.pos.x = MainCamera.pos.x;
	}

	float scale = 4.0f;

	// マリオの表示幅を計算
	float marioWidth = marioImage.sizeX * scale; // マリオの画像の幅を計算
	float marioHeight = marioImage.sizeY * scale;// マリオの画像の高さを計算
	// マリオの右端のワールド座標
	float marioRightX = marioImage.pos.x + marioWidth;

	// ステージの右端を設定 (カメラの最大移動量 + 画面幅)
	float stageRightLimit = 12480.0f + SCREEN_W;

	// マリオがステージの右端を越えないように座標を補正
	if (marioRightX > stageRightLimit)
	{
		marioImage.pos.x = stageRightLimit - marioWidth;
	}

	// 画面上でのマリオの中心X座標を計算し、グローバル変数に保持する
	// デバッグ描画用にマリオのスクリーン矩形を更新しておく
	int mario_screenX = (int)(marioImage.pos.x - MainCamera.pos.x);
	int mario_screenY = (int)(marioImage.pos.y - MainCamera.pos.y);
	mario_centerX = mario_screenX + ((int)marioWidth / 2);

	mario_debug_x1 = mario_screenX;
	mario_debug_y1 = mario_screenY;
	mario_debug_x2 = mario_screenX + (int)marioWidth;
	mario_debug_y2 = mario_screenY + (int)marioHeight;

	// アニメーション用のスクリーン現在座標を渡す
	walkAnimation.x = (float)mario_screenX;
	walkAnimation.y = (float)mario_screenY;

	// 【修正】実際に「移動している速度」に基づいて walkAnimation の更新を行います
	if (std::abs(now_speed_x) > 0.1f)
	{
		walkAnimation.AnimationUpdateLoop(); // 正しく walkAnimation を更新
	}
	else
	{
		// 止まっているときは最初のコマ（立ちポーズ）に戻す
		walkAnimation.currentFrame = 0;
	}
}

void Mario::Render()
{
	// 向きに応じて反転させて描画
	// 【修正】初期化されていない walkAnim や古い DrawRotaGraph2 の重複した描画を全て削除しました。
	if (isLeft)
	{
		walkAnimation.AnimationRenderCenter(true); // 左向き (左右反転してアニメーション描画)
	}
	else
	{
		walkAnimation.AnimationRenderCenter(false); // 右向き (反転なしでアニメーション描画)
	}
}