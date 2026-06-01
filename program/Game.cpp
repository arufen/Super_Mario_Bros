#include "Main.h"
#include "Game.h"
#include "Camera.h"
#include "Debug.h"
#include "Map.h"
#include "Mario.h"
<<<<<<< HEAD
#include "Brick_Block.h"
#include "Question_Block.h"
#include "Hard_Block.h"
#include <vector>
=======
#include "Ground.h"
#include "StageManager.h"
#include <vector>
using namespace std;
>>>>>>> 30f87b127552e9cfc3a361a6981d4f865ff36fa6

extern Camera MainCamera;

// マップ
Map MainMap;

// デバッグ用機・
Debug MainDebug;

//int brickGraph = -1;

std::vector<IBlock*> globalBlocks;

// プレイヤー（マリオ）
Mario MainMario;
<<<<<<< HEAD
=======



>>>>>>> 30f87b127552e9cfc3a361a6981d4f865ff36fa6
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
	int texBrick = LoadGraph("data/brick_block.png");
	int texQuestion = LoadGraph("data/question_block.png");
	int texEmpty = LoadGraph("data/empty_block.png");
	int texHard = LoadGraph("data/hard_block.png");

	int totalBlocks = sizeof(world1_1data) / sizeof(BlockSpawnData);
	for (int i = 0; i < totalBlocks; i++)
	{
		Float2 pixelPos;
		pixelPos.x = world1_1data[i].gridX * 64.0f;
		pixelPos.y = world1_1data[i].gridY * 64.0f;

		if (world1_1data[i].type == BlockType::BRICK)
		{
			BrickBlock* brick = new BrickBlock();
			brick->Init(pixelPos, texBrick);
			globalBlocks.push_back(brick);
		}
		else if (world1_1data[i].type == BlockType::QUESTION)
		{
			QuestionBlock* qblock = new QuestionBlock();
			qblock->Init(pixelPos, texQuestion, texEmpty);
			globalBlocks.push_back(qblock);
		}
		else if (world1_1data[i].type == BlockType::BRICK)
		{
			HardBlock* hblock = new HardBlock();
			hblock->Init(pixelPos, texHard);
			globalBlocks.push_back(hblock);
		}
	}
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
<<<<<<< HEAD
	//// Update all active brick block logic frames (bouncing physics)
	//for (int i = 0; i < MAX_BRICKBLOCKS; i++)
	//{
	//	brick_block[i].Update();
	//}
=======
	
	StageManager::GetInstance().Update(MainMario);
	

>>>>>>> 30f87b127552e9cfc3a361a6981d4f865ff36fa6
	// デバッグ機能（モード切り替えなど）の更新

	for (IBlock* block : globalBlocks)
	{
		block->Update();
	}
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
	for (IBlock* block : globalBlocks)
	{
		block->Render(MainCamera.pos);
	}
	// 一番手前にデバッグ情報を描画
	MainDebug.Render();

	StageManager::GetInstance().Render(MainCamera);

}
//---------------------------------------------------------------------------------
//	終了処理
//---------------------------------------------------------------------------------
void GameExit()
{
	for (IBlock* block : globalBlocks)
	{
		delete block;
	}
	globalBlocks.clear();
}
