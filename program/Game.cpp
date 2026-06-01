#include "Main.h"
#include "Game.h"
#include "Camera.h"
#include "Debug.h"
#include "Map.h"
#include "Mario.h"
#include "Ground.h"
#include "StageManager.h"
#include <vector>
using namespace std;

extern Camera MainCamera;

// マップ
Map MainMap;

// デバッグ用機能
Debug MainDebug;

// プレイヤー（マリオ）
Mario MainMario;



//---------------------------------------------------------------------------------
//	初期化処理
//---------------------------------------------------------------------------------
void GameInit()
{
	// マップの初期化
	MainMap.Init();
	
	// マリオおよびデバッグシステムの初期化
	MainMario.Init();
	MainDebug.Init();
	
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
	
	StageManager::GetInstance().Update(MainMario);
	

	// デバッグ機能（モード切り替えなど）の更新
	MainDebug.Update();
}
//---------------------------------------------------------------------------------
//	描画処理
//---------------------------------------------------------------------------------
void GameRender()
{
    //// 背景となるマップを描画
    //MainMap.Render(MainCamera);
	
	// マップの手前にマリオを描画
	MainMario.Render();
	
	// 一番手前にデバッグ情報を描画
	MainDebug.Render();

	StageManager::GetInstance().Render(MainCamera);

}
//---------------------------------------------------------------------------------
//	終了処理
//---------------------------------------------------------------------------------
void GameExit()
{

}
