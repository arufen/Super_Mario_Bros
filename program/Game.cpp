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
#include <vector>
using namespace std;

extern Camera MainCamera;

// マップ
Map MainMap;

// デバッグ用機・
Debug MainDebug;

//int brickGraph = -1;

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
	/*brick_block[MAX_BRICKBLOCKS].Init();*/
	

	
	//LoadWorld1_1Bricks(brickGraph);

	/*BrickBlock* brick = new BrickBlock();
	brick->Init(Float2(1280.0f, 576.0f), texBrick);
	globalBlocks.push_back(brick);

	QuestionBlock* qblock = new QuestionBlock();
	qblock->Init(Float2(1024.0f, 576.0f), texQuestion, texEmpty);
	globalBlocks.push_back(qblock);

	HardBlock* hblock = new HardBlock();
	hblock->Init(Float2(1408.0f, 768.0f), texHard);
	globalBlocks.push_back(hblock);*/

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
	//// Update all active brick block logic frames (bouncing physics)
	//for (int i = 0; i < MAX_BRICKBLOCKS; i++)
	//{
	//	brick_block[i].Update();
	//}
	
	StageManager::GetInstance().Update();
	
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
	
	// Draw all active bricks in the world
	/*for (int i = 0; i < MAX_BRICKBLOCKS; i++)
	{
		brick_block[i].Render(MainCamera.pos);
	}*/
	
	// 一番手前にデバッグ情報を描画
	MainDebug.Render();

	StageManager::GetInstance().Render(MainCamera);

}
//---------------------------------------------------------------------------------
//	終了処理
//---------------------------------------------------------------------------------
void GameExit()
{
	/*for (IBlock* block : globalBlocks)
	{
		delete block;
	}*/
	//globalBlocks.clear();
}
