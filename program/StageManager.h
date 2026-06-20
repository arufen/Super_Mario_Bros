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
#include "Firebar.h"
#include "Super_Mushroom.h"
#include "1-Up_Mushroom.h"
#include "Fire_Flower.h"
#include "Super_Star.h"

//Enemies
#include "Goomba.h"
#include "Koopa_Troopa.h"

#include "Goal_Pole.h"
#include "Castle.h"

using namespace std;

enum class Stage
{
	WORLD_1_1,
	WORLD_1_4,
};

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

	// ゴールポール
	vector<GoalPole*> goalPoles;
	vector<Castle*> castles;

	//Enemy （敵）
	vector<Goomba*> goomba;
	vector<KoopaTroopa*> koopaTroopa;

	// returns all collidable blocks for mario to register
	vector<Collidable*> GetCollidables();

	vector<Pipe*> globalPipes;
	vector<UnderWorldPipe*> underWorldPipe;
	vector<IBlock*> hardPipe;

	vector<BrickBlockParts*> brickParts;

	WorldZone currentzone;

	//for all blocks
	vector<IBlock*> overworld1_1Blocks;
	vector<IBlock*> overworld1_4Blocks;
	vector<IBlock*> underworldBlocks;
	vector<IBlock*> globalBlocks;

	//Collectable Item
	vector<Coin*> coins;
	vector<SuperMushroom*> superMushroom;
	vector<UpMushroom*> upMushroom;
	vector<FireFlower*> fireFlower;
	vector<SuperStar*> superStar;


	Image background;

	//Cleanup all vectors
	void ClearStage();

	//Main thread 
	void Init(Stage stageNumber);	//Load blocks (ブロックの初期化）
	void Update(Camera& camera);	//Check Collision （当たり判定チェック）
	void Render(Camera& camera); //Camera is for render globally (グロバール座標を使うためにカメラが必要）

	void TransferWorldZone(WorldZone newZone); // handle world zone transitions (ワールドゾーンの切り替えを処理)
	bool GetIsUnderworld() const { return currentzone == WorldZone::UNDERWORLD; }

	//void addBrickParts(const BrickBlockParts& piece) { brickParts.push_back(piece); }
	int GetBrickPartsHandle() const {return tex_brickParts;}

	// 現在のワールドゾーン（地上/地下）を返す関数
	WorldZone GetWorldZone() const { return currentzone; }

private:
	//singleton pattern(シングルトンパターン）
	StageManager() : currentzone(WorldZone::OVERWORLD){}
	~StageManager() {};
	int tex_brickParts;
};