#pragma once

#include "Mario.h"
#include "Camera.h"
#include <vector>
#include "HelpfulFunc.h"

#include "Collidable.h" //To check collision for every block

//All Blocks in the stage
#include "Ground.h"
#include "IBlock.h"
#include "Warp_Pipe.h"
#include "Axe.h"

//ITEMS
#include "Coin.h"
#include "Firebar.h"
#include "Super_Mushroom.h"
#include "Fire_Flower.h"
#include "Super_Star.h"

//Enemies
#include "Goomba.h"
#include "Koopa_Troopa.h"
#include "Bowser.h"
#include "Bowser_Fire.h"

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



	// ゴールポール
	vector<GoalPole*> goalPoles;
	vector<Castle*> castles;

	//Enemy （敵）
	vector<Goomba*> goomba;
	vector<KoopaTroopa*> koopaTroopa;
	vector<Bowser*> bowser;
	vector<BowserFire*> bowserFire;

	// returns all collidable blocks for mario to register
	vector<Collidable*> GetCollidables();

	vector<Pipe*> globalPipes;
	vector<UnderWorldPipe*> underWorldPipe;
	vector<IBlock*> hardPipe;

	WorldZone currentzone;

	//for all blocks
	vector<Ground> ground;
	vector<Ground> underworld_ground;
	vector<IBlock*> overworld1_1Blocks;
	vector<IBlock*> overworld1_4Blocks;
	vector<IBlock*> underworldBlocks;
	vector<IBlock*> globalBlocks;
	vector<Ground*> bridge;
	vector<Axe*> axe; //isTrigger true

	//Collectable Item
	vector<Coin*> coins;
	vector<SuperMushroom*> superMushroom;
	vector<FireFlower*> fireFlower;
	vector<SuperStar*> superStar;

	Stage currentStage{ Stage::WORLD_1_1 };

	Image background;

	//Bridge
	Timer timerClearingBridgeInterval{ 0.1f };
	bool isBridgeClearing; 
	bool isBossDefeat; //true if boss defeated with fireballs
	void ClearBridge();


	//Cleanup all vectors
	void ClearStage();

	//Main thread 
	void Init(Stage stageNumber);	//Load blocks (ブロックの初期化）
	void Update(Camera& camera);	//Check Collision （当たり判定チェック）
	void Render(Camera& camera); //Camera is for render globally (グロバール座標を使うためにカメラが必要）

	void TransferWorldZone(WorldZone newZone); // handle world zone transitions (ワールドゾーンの切り替えを処理)
	bool GetIsUnderworld() const { return currentzone == WorldZone::UNDERWORLD; }

	// 現在のワールドゾーン（地上/地下）を返す関数
	WorldZone GetWorldZone() const { return currentzone; }

private:
	//singleton pattern(シングルトンパターン）
	StageManager() : currentzone(WorldZone::OVERWORLD){}
	~StageManager() {};
};