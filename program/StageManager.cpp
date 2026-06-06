#include "StageManager.h"

//Blocks
#include "Ground.h"

#include "Brick_Block.h"
#include "Question_Block.h"
#include "Hard_Block.h"

//Enemis
#include "Goomba.h"
using namespace std;



void CreateGoomba(int x, int y)
{
	Goomba newGoomba;
	newGoomba.Init(x * BLOCK_SIZE, y * BLOCK_SIZE, LoadGraph("data/image/goomba.png"));
	StageManager::GetInstance().goomba.push_back(newGoomba);
}

//Create ground tiles from (fromTileX, fromTileY) to (toTileX, toTileY) (max Y: 14)
void CreateGrounds(int fromTileX, int fromTileY, int toTileX, int toTileY)
{
	for (int x = fromTileX; x <= toTileX; x++)
	{
		for (int y = fromTileY; y <= toTileY; y++)
		{
			Ground newGround;
			newGround.Init(x * BLOCK_SIZE, y * BLOCK_SIZE, LoadGraph("data/image/ground.png"));
			StageManager::GetInstance(). ground.push_back(newGround);
		}
	}
}

//Stair looking ground (ŠK’i‚ðì‚éŠÖ”j
void CreateStairs(int fromTileX, int fromTileY, int toTileX, int toTileY, bool flipFlag)
{

	//invalid if Y > X
	int deltaX = toTileX - fromTileX;
	int deltaY = toTileY - fromTileY;

	if (deltaY > deltaX)
	{
		return;
	}

	int handle = LoadGraph("data/image/ground2.png");

	

	if (!flipFlag)
	{
		int a = fromTileX;

		for (int y = toTileY; y >= fromTileY; y--)
		{

			for (int x = toTileX; x >= a; x--)
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
		for (int y = toTileY; y >= fromTileY; y--)
		{
			for (int x = a; x >= fromTileX; x--)
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
	CreateGrounds(0, 13, 68, 14); //from (0, 13) to (20, 14) | (0, 13)‚©‚ç(68, 14)‚Ü‚Å
	CreateGrounds(71, 13, 85, 14);
	CreateGrounds(88, 13, 152, 14);
	CreateGrounds(155, 13, 210, 14);

	//Stairs
	CreateStairs(134, 9, 137, 12, false);
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

	CreateStairs(140, 9, 143, 12, true);

	CreateStairs(148, 9, 152, 12, false);

	CreateStairs(155, 9, 158, 12, true);

	CreateStairs(181, 5, 189, 12, false);

	//goomba test
	CreateGoomba(10, 4);
	CreateGoomba(12, 6);
	CreateGoomba(15, 6);

	//tmp
	CreateGrounds(20, 12, 21, 12);
}

void StageManager::Update()
{

	//Goomba
	for (int i = 0; i < goomba.size(); i++)
	{
		goomba[i].Update();
	}
	for (IBlock* block : globalBlocks)
	{
		block->Update();
	}
	for (Pipe* pipe : globalPipes)
	{
		pipe->Update();
	}
}

void StageManager::Render(Camera& camera)
{
	//Stage
	for (int i = 0; i < ground.size(); i++)
	{
		ground[i].RenderGlobal(camera);
	}

	//Goomba
	for (int i = 0; i < goomba.size(); i++)
	{
		goomba[i].RenderGlobal(camera);

		//DrawFormatString(100, 100 + 20 * i, GetColor(255, 255, 255), "speed: %f", goomba[i].now_speed_x);
	}
	for (IBlock* block : globalBlocks)
	{
		block->Render(camera);
	}
	for (Pipe* pipe : globalPipes)
	{
		pipe->Render(camera);
	}

}