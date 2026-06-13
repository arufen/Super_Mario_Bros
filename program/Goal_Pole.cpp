#include "Goal_Pole.h"
#include "Mario.h"
#include "Debug.h"
#include "DxLib.h"
#include "Time.h"
#include "Sound.h"

GoalPole::GoalPole() : isReached(false), maxFlagY(0.0f) {}

void GoalPole::Init(float x, float y)
{
	image.InitialImageAndSize(LoadGraph("data/image/goal_pole.png"));
	image.pos.Set(x, y);

	flagImage.InitialImageAndSize(LoadGraph("data/image/flag.png"));

	// 旗の位置を調整（ポールの左側に配置し、Y座標はてっぺん付近に）
	// ※画像のサイズやポールの太さに合わせて、+ や - の数値を微調整してください
	float flagX = x - (float)flagImage.sizeX + 32.0f; // ポールの左側に少し食い込ませる例
	float flagY = y + 35.0f; // ポールのてっぺんから少し下げる
	flagImage.pos.Set(flagX, flagY);

	// 土台（四角いボックス）の高さを指定
	float baseHeight = 64.0f;

	// ポールの当たり判定の幅を指定
	float poleWidth = 12.0f;

	// 当たり判定を画像の中央に配置するためのX座標のズレ（オフセット）を計算
	float poleOffsetX = ((float)image.sizeX - poleWidth) / 2.0f;

	// 旗が下りる限界（土台のすぐ上）のY座標を計算しておく
	maxFlagY = y + (float)image.sizeY - baseHeight - (float)flagImage.sizeY;

	// ① ポール部分（すり抜ける＋クリア判定）の当たり判定
	collider = Collider(x + poleOffsetX, y, poleWidth, (float)image.sizeY - baseHeight);
	isTrigger = true;

	// ② 土台部分（弾かれる）の当たり判定
	// Y座標を下にずらし、高さを土台分に設定
	baseCollider.collider = Collider(x, y + (float)image.sizeY - baseHeight, (float)image.sizeX, baseHeight);
}

void GoalPole::Update()
{
	// マリオがポールに触れたら旗を下ろすアニメーション
	if (isReached)
	{
		if (flagImage.pos.y < maxFlagY)
		{
			flagImage.pos.y += 4.0f; // 1フレームに4ピクセルずつ下ろす（好みに合わせて速度調整してください）

			// 行き過ぎ防止
			if (flagImage.pos.y > maxFlagY)
			{
				flagImage.pos.y = maxFlagY;
			}
		}
	}
}

void GoalPole::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderImage(image);

	camera.GlobalRenderImage(flagImage);

	// デバッグモードの時だけ、緑色の線で当たり判定を可視化する
	if (map_mode == MODE_DEBUG)
	{
		// ポール部分（トリガー）を緑色で描画
		camera.GlobalRenderBox(collider.x, collider.y, collider.x + collider.width, collider.y + collider.height, GetColor(0, 255, 0), FALSE);

		// 土台部分（ソリッド）を赤色で描画
		camera.GlobalRenderBox(baseCollider.collider.x, baseCollider.collider.y, baseCollider.collider.x + baseCollider.collider.width, baseCollider.collider.y + baseCollider.collider.height, GetColor(255, 0, 0), FALSE);
	}
}

// どこから当たってもクリア判定にしたいので、全て OnHitSide に流す
void GoalPole::OnHitTop(RigidBody& player) { OnHitSide(player); }
void GoalPole::OnHitBottom(RigidBody& player) { OnHitSide(player); }

void GoalPole::OnHitSide(RigidBody& player)
{
	if (isReached) return;

	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		isReached = true;

		// クリア時の処理
		g_IsTimerStopped = true; // タイムストップ

		// BGMを止める（クリアファンファーレ音源がある場合はここで鳴らす）
		SoundManager::GetInstance().StopBGM();
		// SoundManager::GetInstance().PlaySE("Clear"); // 用意があればコメントアウトを外す

		// マリオの動きを止める（ポールを降りる演出などに繋ぐため）
		mario->now_speed_x = 0.0f;
		mario->now_speed_y = 0.0f;
	}
}