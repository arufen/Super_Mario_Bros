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

	StageManager::GetInstance().Init();

	// register all collidables into mario
	// (StageManager probably owns the blocks, so grab them from there)
	for (auto* block : StageManager::GetInstance().GetCollidables())
	{
		MainMario.collidables.push_back(block);
	}
}
//---------------------------------------------------------------------------------
//	更新処理
//---------------------------------------------------------------------------------
void GameUpdate()
{
	MainCamera.Update();
	MainMario.Update();

	// マリオが死亡状態（DEAD）の場合の処理
	if (MainMario.GetState() == MarioState::DEAD)
	{
		// マリオが画面（SCREEN_H）の下に完全に落ちきったか判定
		// マリオの現在の画面Y座標 = ワールド座標Y - カメラ座標Y
		float marioScreenY = MainMario.position.y - MainCamera.pos.y;

		// 画面下に消え去ったら、ゲームを最初からリスタートする
		if (marioScreenY > SCREEN_H + 100)
		{
			GameInit(); // 初期化関数をもう一度呼び出して最初からやり直す
		}

		// マリオが死んでいる間は、タイムや敵（StageManager）の更新をスキップして完全にストップさせる
		return;
	}

	MainTime.Update();
	StageManager::GetInstance().Update();
	

	// デバッグ機能（モード切り替えなど）の更新
	MainDebug.Update();
}
//---------------------------------------------------------------------------------
//	描画処理
//---------------------------------------------------------------------------------
void GameRender()
{
	
	// マップの手前にマリオを描画
	MainMario.Render();

	// マリオの手前に残り時間を描画
	MainTime.Render();

	StageManager::GetInstance().Render(MainCamera);

	// 一番手前にデバッグ情報を描画
	MainDebug.Render();
}
//---------------------------------------------------------------------------------
//	終了処理
//---------------------------------------------------------------------------------
void GameExit()
{
}
