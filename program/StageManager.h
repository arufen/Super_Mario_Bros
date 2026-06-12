#pragma once

#include "Mario.h"
#include "Camera.h"
#include <vector>

#include "Collidable.h" //To check collision for every block

//All Blocks in the stage
#include "Ground.h"
#include "IBlock.h"
#include "Warp_Pipe.h"

//ITEMS
#include "Coin.h"
#include "Super_Mushroom.h"

//Enemies
#include "Goomba.h"

using namespace std;

class StageManager
{
public:
	static StageManager& GetInstance()
	{
		static StageManager instance;
		return instance;
	}

	//All register blocks
	//Ground （床）
	vector<Ground> ground;

	vector<Ground> underworld_ground;

	//Enemy （敵）
	vector<Goomba*> goomba;

	// returns all collidable blocks for mario to register
	vector<Collidable*> GetCollidables();

	vector<Pipe*> globalPipes;
	vector<UnderWorldPipe*> underWorldPipe;
	vector<IBlock*> hardPipe;

	WorldZone currentzone;

	//for all blocks
	vector<IBlock*> overworld1_1Blocks;
	vector<IBlock*> underworldBlocks;
	vector<IBlock*> globalBlocks;

	//Collectable Item
	vector<Coin*> coins;
	vector<SuperMushroom*> superMushroom;

	Image background;

	//Main thread 
	void Init();	//Load blocks (ブロックの初期化）
	void Update(Camera& camera);	//Check Collision （当たり判定チェック）
	void Render(Camera& camera); //Camera is for render globally (グロバール座標を使うためにカメラが必要）

	void TransferWorldZone(WorldZone newZone); // handle world zone transitions (ワールドゾーンの切り替えを処理)
	bool GetIsUnderworld() const { return currentzone == WorldZone::UNDERWORLD; }

private:
	//singleton pattern(シングルトンパターン）
	StageManager() : currentzone(WorldZone::OVERWORLD){}
	~StageManager() {};
};