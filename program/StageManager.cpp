#include "StageManager.h"

//Blocks
#include "Ground.h"

#include "Brick_Block.h"
#include "Question_Block.h"
#include "Hard_Block.h"

//Enemis
#include "Goomba.h"
using namespace std;


template <typename T1, typename T2>
void CreateGoomba(T1 x, T2 y)
{
	Goomba* newGoomba = new Goomba();
	newGoomba->Init(x * BLOCK_SIZE, y * BLOCK_SIZE, LoadGraph("data/image/goomba.png"));
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

void CreateCoin(float x, float y)
{
	Coin* newCoin = new Coin();
	newCoin->Init(x * BLOCK_SIZE, y * BLOCK_SIZE, LoadGraph("data/image/coin.png"));
	StageManager::GetInstance().coins.push_back(newCoin);
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

	for (auto* g : goomba)
	{
		result.push_back(g);
	}
	for (auto* g : coins)
	{
		result.push_back(g);
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
	
	//ENEMIES
	//GOOMBA
	CreateGoomba(22, 12);
	CreateGoomba(42, 12);
	CreateGoomba(52, 12);
	CreateGoomba(53.5, 12);

	CreateGoomba(80, 4);
	CreateGoomba(81.5, 4);


	CreateGoomba(98, 12);
	CreateGoomba(99.5f, 12);


	CreateGoomba(115, 12);
	CreateGoomba(116.5f, 12);


	CreateGoomba(125, 12);
	CreateGoomba(126.5f, 12);
	CreateGoomba(129, 12);
	CreateGoomba(130.5f, 12);

	CreateGoomba(174, 12);
	CreateGoomba(175.5f, 12);

	//COINS
	CreateCoin(15, 10);

	//BACKGROUND
	background.InitialImageAndSize(LoadGraph("data/image/1-1_background.png"));
	background.pos.Set(0.0f, 0.0f);

}

void StageManager::Update(Camera& camera)
{

	//Goomba
	for (int i = 0; i < goomba.size(); i++)
	{
		goomba[i]->Update(camera);

		if (goomba[i]->state == EnemyState::DEAD)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), goomba[i]),
				RigidBody::collidables.end()
			);
			goomba.erase(goomba.begin() + i);
		}
	}
	for (IBlock* block : globalBlocks)
	{
		block->Update();
	}
	for (Pipe* pipe : globalPipes)
	{
		pipe->Update();
	}

	//Coin
	for (int i = 0; i < coins.size(); i++)
	{
		if (!coins[i]->active)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), coins[i]),
				RigidBody::collidables.end()
			);
			//remove coins from stage manager
			coins.erase(coins.begin() + i);
		}
	}

}

void StageManager::Render(Camera& camera)
{


	//BLOCKS
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

	for (Coin* coin: coins)
	{
		if (coin->active)
		{
			coin->RenderGlobal(camera);
		}
	
	}

	//ENEMIES
	//Goomba
	for (int i = 0; i < goomba.size(); i++)
	{
		goomba[i]->RenderGlobal(camera);

		//DrawFormatString(0, 60 + 20 * i, GetColor(255, 255, 255), "goomba pos x: %f, y: %f", goomba[i].position.x, goomba[i].position.y);
	}

}