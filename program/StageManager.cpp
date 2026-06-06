#include "StageManager.h"
#include "Game.h"
#include <vector>

//Blocks
#include "Ground.h"

#include "Brick_Block.h"
#include "Question_Block.h"
#include "Hard_Block.h"

//Enemis
#include "Goomba.h"
using namespace std;

Goomba testGoomba;

void CreateGoomba(int x, int y)
{

	testGoomba.Init(x * BLOCK_SIZE, y * BLOCK_SIZE, LoadGraph("data/goomba.png"));
}

//Create ground tiles from (fromTileX, fromTileY) to (toTileX, toTileY) (max Y: 14)
void CreateGrounds(int fromTileX, int fromTileY, int toTileX, int toTileY)
{
	for (int x = fromTileX; x <= toTileX; x++)
	{
		for (int y = fromTileY; y <= toTileY; y++)
		{
			Ground newGround;
			newGround.Init(x * BLOCK_SIZE, y * BLOCK_SIZE, LoadGraph("data/ground.png"));
			StageManager::GetInstance(). ground.push_back(newGround);
		}
	}
}

//Stair looking ground (階段を作る関数）
void CreateStairs(int fromTileX, int fromTileY, int toTileX, int toTileY, bool flipFlag)
{
	int handle = LoadGraph("data/ground2.png");

	

	if (!flipFlag)
	{
		int a = fromTileX;

		for (int y = toTileY; y > fromTileY; y--)
		{

			for (int x = toTileX; x > a; x--)
			{
				Ground newGround;
				newGround.Init(x * BLOCK_SIZE, y * BLOCK_SIZE, handle);
				StageManager::GetInstance().ground.push_back(newGround);
			}
			a++;
		}
	}
	else
	{
		int a = toTileX;
		for (int y = toTileY; y > fromTileY; y--)
		{
			for (int x = a; x > fromTileX; x--)
			{
				Ground newGround;
				newGround.Init(x * BLOCK_SIZE, y * BLOCK_SIZE, handle);
				StageManager::GetInstance().ground.push_back(newGround);
			}
			a--;
		}
	}

}

vector<Collidable*> StageManager::GetCollidables()
{
	// returns all collidable blocks for mario to register
	
	vector<Collidable*> result;
	for (auto& g : ground)
		result.push_back(&g);
	/*for (auto& g : brick_block)
		result.push_back(&g);*/
	for (auto* block : globalBlocks) {
		if (block->IsActive()) {
			Collidable* physicsObject = dynamic_cast<Collidable*>(block);
			if (physicsObject != nullptr) {
				result.push_back(physicsObject);
			}
		}
	}
	for (auto* pipe : globalPipes) {
		if (pipe->IsActive()) {
			result.push_back(pipe); // Makes the pipe solid to Mario!
		}
	}
	// when u add pipe, questionblock etc just do:
	// for (auto& p : pipes)
	//     result.push_back(&p);
	return result;
}

void StageManager::Init()
{
	//Stage (ground1)
	CreateGrounds(0, 13, 68, 14); //from (0, 13) to (20, 14) | (0, 13)から(68, 14)まで
	CreateGrounds(71, 13, 85, 14);
	CreateGrounds(88, 13, 152, 14);
	CreateGrounds(155, 13, 210, 14);

	int texBrick = LoadGraph("data/brick_block.png");
	int texQuestion = LoadGraph("data/question_block.png");
	int texEmpty = LoadGraph("data/empty_block.png");
	int texHard = LoadGraph("data/hard_block.png");

	int tLeft = LoadGraph("data/top_left.png");
	int tRight = LoadGraph("data/top_right.png");
	int bLeft = LoadGraph("data/bottom_left.png");
	int bRight = LoadGraph("data/bottom_right.png");

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
			qblock->Init(pixelPos, texQuestion, texHard);
			globalBlocks.push_back(qblock);
		}
		else if (world1_1data[i].type == BlockType::HARD)
		{
			HardBlock* hblock = new HardBlock();
			hblock->Init(pixelPos, texHard);
			globalBlocks.push_back(hblock);
		}
		else if (world1_1data[i].type == BlockType::PIPE)
		{
			Pipe* myPipe = new Pipe();
			myPipe->Init(pixelPos, tLeft, tRight, bLeft, bRight, world1_1data[i].pipeHeight, world1_1data[i].isWarpPipe);

			globalPipes.push_back(myPipe); // Save it to your vector!
		}
	}

	// Spawn a standard pipe that is 2 blocks tall at Grid X = 28, Grid Y = 11
	//Float2 pipePos;
	//pipePos.x = 28 * 64.0f;
	//pipePos.y = 11 * 64.0f;

	//Pipe* myPipe = new Pipe();
	//myPipe->Init(pipePos, tLeft, tRight, bLeft, bRight, 2); // 2 means 2 tiles tall

	//globalPipes.push_back(myPipe); // Save it to your vector!
	////Stairs
	//CreateStairs(134, 9, 137, 12, false);

	//CreateStairs(140, 9, 153, 12, true);

	//CreateStairs(148, 9, 152, 12, false);

	//CreateStairs(155, 9, 158, 12, true);

	//CreateStairs(181, 5, 189, 12, false);
	
	//test stairs
	CreateStairs(11, 11, 13, 12, false);

	////tmp 
	//CreateGrounds(20, 9, 25, 9);
	//CreateGrounds(10, 12, 15, 12);

	//goomba test
	CreateGoomba(10, 4);
}

void StageManager::Update()
{
	////Ground Check collision (当たり判定チェック）
	//for (int i = 0; i < ground.size(); i++)
	//{
	//	ground[i].Update(mario);
	//}
	for (IBlock* block : globalBlocks)
	{
		block->Update();
	}
	for (Pipe* pipe : globalPipes)
	{
		pipe->Update();
	}
	//test goomba
	testGoomba.Update();
}

void StageManager::Render(Camera& camera)
{
	//Stage
	for (int i = 0; i < ground.size(); i++)
	{
		ground[i].RenderGlobal(camera);
	}
	for (IBlock* block : globalBlocks)
	{
		block->Render(camera);
	}
	for (Pipe* pipe : globalPipes)
	{
		pipe->Render(camera);
	}
	//goomba test
	testGoomba.GlobalRender(camera);
}