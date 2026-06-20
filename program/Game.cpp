#include "Main.h"
#include "Game.h"
#include "Camera.h"
#include "Debug.h"
#include "Map.h"
#include "Mario.h"
//#include "Brick_Block.h"
//#include "Question_Block.h"
//#include "Hard_Block.h"
#include <vector>
#include "Ground.h"
#include "StageManager.h"
#include "Time.h"
#include "Sound.h"
#include "Score.h"
#include "Fonts.h"
using namespace std;

extern Camera MainCamera;

// マップ
Map MainMap;

// デバッグ用機・
Debug MainDebug;

//int brickGraph = -1;

// プレイヤー（マリオ）
Mario MainMario;

// ゲーム時間
GameTime MainTime;

//---------------------------------------------------------------------------------
//	初期化処理
//---------------------------------------------------------------------------------
void GameInit()
{
	SoundManager::GetInstance().Init(); // サウンドの読み込み

	// BGMを鳴らし始める
	SoundManager::GetInstance().PlayBGM("Stage1");

	MarioFont::GetInstance().Init();
	MainMap.Init();

	// マップの初期化
	MainMap.Init();
	
	// マリオおよびデバッグシステムの初期化
	MainMario.Init();

	MainDebug.Init();

	// ゲーム時間の初期化
	MainTime.Init();

	// カメラの初期位置を設定
	MainCamera.pos.Set(0.0f, 0.0f);

	StageManager::GetInstance().Init(Stage::WORLD_1_1);

	// register all collidables into for every object that has RigidBody
	for (auto* block : StageManager::GetInstance().GetCollidables())
	{
		RigidBody::collidables.push_back(block);
	}
}
//---------------------------------------------------------------------------------
//	更新処理
//---------------------------------------------------------------------------------
void GameUpdate()
{
	if (StageManager::GetInstance().currentzone == WorldZone::UNDERWORLD)
	{
		MainCamera.pos.x = 56 * BLOCK_SIZE;
		MainCamera.pos.y = 15 * BLOCK_SIZE;
	}
	else
	{
		MainCamera.Update();
	}

	MainMario.Update();

	// マリオが死亡状態なら、これ以降の更新をすべてスキップしてフリーズさせる
	if (MainMario.GetState() == MarioState::DEAD)
	{
		// タイムの進行や、StageManager（クリボーの移動など）のUpdateを通さない
		return;
	}

	MainTime.Update();
	StageManager::GetInstance().Update(MainCamera);
	
	MainCamera.Update();
	// デバッグ機能（モード切り替えなど）の更新
	MainDebug.Update();

	if (PushHitKey(KEY_INPUT_S) && CheckHitKey(KEY_INPUT_LCONTROL))
	{
		//test
		StageManager::GetInstance().ClearStage();
		StageManager::GetInstance().Init(Stage::WORLD_1_4);
		StageManager::GetInstance().currentStage == Stage::WORLD_1_4;

		//Change time 
		MainTime.SetCount(300);

		// register all collidables into for every object that has RigidBody
		for (auto* block : StageManager::GetInstance().GetCollidables())
		{
			RigidBody::collidables.push_back(block);
		}
	}
}
//---------------------------------------------------------------------------------
//	描画処理
//---------------------------------------------------------------------------------
void GameRender()
{
	// 1. 背景の描画
	MainCamera.GlobalRenderImage(StageManager::GetInstance().background);

	// 2. 地下なら画面全体を真っ黒に塗りつぶす（背景を上書き）
	if (StageManager::GetInstance().currentzone == WorldZone::UNDERWORLD)
	{
		DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), TRUE);
	}

	// 3 & 4. 状態による描画順（手前・奥）の切り替え
	if (MainMario.GetState() == MarioState::WARPING)
	{
		// ワープ時：土管の後ろに隠すため、マリオを先に（奥）、ステージを後に（手前）描画する
		MainMario.Render();
		StageManager::GetInstance().Render(MainCamera);
	}
	else
	{
		// 通常時・死亡時：マリオが隠れないよう、ステージを先に（奥）、マリオを後に（手前）描画する
		StageManager::GetInstance().Render(MainCamera);
		MainMario.Render();
	}
	
	// 5. UI（文字や情報）の描画（すべての上に重ねるため一番最後に持ってくる）
	
	// マリオの手前に残り時間を描画
	//MainTime.Render();
	MarioFont::GetInstance().DrawMarioLabel(80, 20);
	MarioFont::GetInstance().DrawNumber(80, 52, MainMario.GetScore(), 6);

	MarioFont::GetInstance().DrawTimeLabel(SCREEN_W - 232, 20);
	MarioFont::GetInstance().DrawWorldLabel(SCREEN_W - 482, 20);

	MarioFont::GetInstance().DrawNumber(SCREEN_W - 200, 60, MainTime.count, 3);

	// コイン数の描画
	DrawFormatString(0, 80, GetColor(255, 255, 255), "Mario coin : %d", MainMario.coin);

	// 一番手前にデバッグ情報を描画
	MainDebug.Render();

	// debug用の当たり判定枠など
	if (map_mode == MODE_DEBUG)
	{
		for (auto& c : RigidBody::collidables)
		{
			c->Render(MainCamera);
		}
	}

	DrawString(0, 100, "CTRL + S でステージを変える", GetColor(200, 0, 0));
}
//---------------------------------------------------------------------------------
//	終了処理
//---------------------------------------------------------------------------------
void GameExit()
{
	MarioFont::GetInstance().Exit();
}
