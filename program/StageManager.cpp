#include "StageManager.h"

using namespace std;



void CreateGoomba(int x, int y)
{
	Goomba newGoomba;
	newGoomba.Init(x * BLOCK_SIZE, y * BLOCK_SIZE, LoadGraph("data/goomba.png"));
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

//Stair looking ground (階段を作る関数）
void CreateStairs(int fromTileX, int fromTileY, int toTileX, int toTileY, bool flipFlag)
{

	//invalid if Y > X
	int deltaX = toTileX - fromTileX;
	int deltaY = toTileY - fromTileY;

	if (deltaY > deltaX)
	{
		return;
	}

	int handle = LoadGraph("data/ground2.png");

	

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

	//Stairs
	CreateStairs(134, 9, 137, 12, false);

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
	////Ground Check collision (当たり判定チェック）
	//for (int i = 0; i < ground.size(); i++)
	//{
	//	ground[i].Update(mario);
	//}

	//Goomba
	for (int i = 0; i < goomba.size(); i++)
	{
		goomba[i].Update();
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

}