#include "StageManager.h"
#include "Ground.h"
#include "Game.h"
#include <vector>
using namespace std;



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

vector<Collidable*> StageManager::GetCollidables()
{
	// returns all collidable blocks for mario to register
	
	vector<Collidable*> result;
	for (auto& g : ground)
		result.push_back(&g);
	// when u add pipe, questionblock etc just do:
	// for (auto& p : pipes)
	//     result.push_back(&p);
	return result;
	
}

void StageManager::Init()
{
	//Stage
	CreateGrounds(0, 13, 68, 14); //from (0, 13) to (20, 14) | (0, 13)から(68, 14)まで
	CreateGrounds(71, 13, 85, 14);
	CreateGrounds(89, 13, 153, 14);
	CreateGrounds(156, 13, 207, 14);

	//tmp 
	CreateGrounds(20, 9, 25, 9);
	CreateGrounds(10, 12, 15, 12);
}

void StageManager::Update(Mario& mario)
{
	////Ground Check collision (当たり判定チェック）
	//for (int i = 0; i < ground.size(); i++)
	//{
	//	ground[i].Update(mario);
	//}
}

void StageManager::Render(Camera& camera)
{
	//Stage
	for (int i = 0; i < ground.size(); i++)
	{
		ground[i].RenderGlobal(camera);
	}
}