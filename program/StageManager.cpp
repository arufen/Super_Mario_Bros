#include "StageManager.h"

//Blocks
#include "Ground.h"
#include "Brick_Block.h"
#include "Question_Block.h"
#include "Hard_Block.h"
#include "Hidden_Block.h"

//Enemis
#include "Goomba.h"
#include "Fonts.h"
#include "Sound.h"

using namespace std;
extern Camera MainCamera;
extern Mario MainMario;

// Inside StageManager.cpp
void StageManager::TransferWorldZone(WorldZone newZone)
{
	currentzone = newZone;

	if (currentzone == WorldZone::OVERWORLD)
	{
		MainCamera.pos.x = 160 * BLOCK_SIZE;
		MainCamera.pos.y = 0;

		float spawnX = (164 * BLOCK_SIZE) - ((float)MainMario.small_mario_waitImage.sizeX / 2.0f);
		float spawnY = 11 * BLOCK_SIZE + 32.0f;

		MainMario.now_speed_x = 0.0f;
		MainMario.now_speed_y = 0.0f;
		MainMario.isJumping = false;

		//MainMario.Warping(spawnX, spawnY, Mario::WarpDir::UP);
		MainMario.WarpPosition(spawnX, spawnY);
		SoundManager::GetInstance().PlaySE("Warp");
	}
	else if (currentzone == WorldZone::UNDERWORLD)
	{
		MainMario.position.Set(57 * BLOCK_SIZE, 15 * BLOCK_SIZE);
		MainMario.RigidBody_collider.x = MainMario.position.x;
		MainMario.RigidBody_collider.y = MainMario.position.y;
		MainMario.now_speed_y = 0.0f;
	}
}

//ENEMIES
template <typename T1, typename T2>
void CreateGoomba(T1 x, T2 y)
{
	Goomba* newGoomba = new Goomba();
	newGoomba->Init(x * BLOCK_SIZE, y * BLOCK_SIZE, LoadGraph("data/image/GoombaAnimation.png"));
	StageManager::GetInstance().goomba.push_back(newGoomba);
}

template <typename T1, typename T2>
void CreateKoopaTroopa(T1 x, T2 y)
{
	KoopaTroopa* newKoopaTroopa = new KoopaTroopa();
	newKoopaTroopa->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
	StageManager::GetInstance().koopaTroopa.push_back(newKoopaTroopa);
}

template <typename T1, typename T2>
void CreateBowser(T1 x, T2 y)
{
	Bowser* newBowser = new Bowser();
	newBowser->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
	StageManager::GetInstance().bowser.push_back(newBowser);
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
			StageManager::GetInstance().ground.push_back(newGround);
		}
	}
}

//Create Stationary block that has no unique feature with a custom handle

void CreateGrounds(int fromTileX, int fromTileY, int toTileX, int toTileY, int handle)
{
	for (int x = fromTileX; x <= toTileX; x++)
	{
		for (int y = fromTileY; y <= toTileY; y++)
		{
			Ground newGround;
			newGround.Init(x * BLOCK_SIZE, y * BLOCK_SIZE, handle);
			StageManager::GetInstance().ground.push_back(newGround);
		}
	}
}

//Create bridge in 1-4
void CreateBridge(int fromTileX, int fromTileY, int toTileX, int toTileY, int handle)
{
	for (int x = fromTileX; x <= toTileX; x++)
	{
		for (int y = fromTileY; y <= toTileY; y++)
		{
			Ground* newBridge = new Ground();
			newBridge->Init(x * BLOCK_SIZE, y * BLOCK_SIZE, handle);
			StageManager::GetInstance().bridge.push_back(newBridge);
		}
	}
}

//Stair looking ground
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
	newCoin->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
	StageManager::GetInstance().coins.push_back(newCoin);
}
//Create coin tiles from (fromTileX, fromTileY) to (toTileX, toTileY) (max Y: 14)
void CreateCoin(int fromTileX, int fromTileY, int toTileX, int toTileY)
{
	for (int x = fromTileX; x <= toTileX; x++)
	{
		for (int y = fromTileY; y <= toTileY; y++)
		{
			Coin* newCoin = new Coin();
			newCoin->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
			StageManager::GetInstance().coins.push_back(newCoin);
		}
	}
}

//ITEMS
void StageManager::CreateSuperMushroom(float x, float y)
{
	SuperMushroom* newSuperMushroom = new SuperMushroom();
	newSuperMushroom->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
	newSuperMushroom->SpawnItem();
	StageManager::GetInstance().superMushroom.push_back(newSuperMushroom);
	RigidBody::collidables.push_back(newSuperMushroom);
}

void StageManager::CreateFireFlower(float x, float y)
{
	FireFlower* newFireFlower = new FireFlower();
	newFireFlower->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
	newFireFlower->SpawnItem();
	StageManager::GetInstance().fireFlower.push_back(newFireFlower);
	RigidBody::collidables.push_back(newFireFlower);

}

void StageManager::CreateSuperStar(float x, float y)
{
	SuperStar* newSuperStar = new SuperStar();
	newSuperStar->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
	newSuperStar->SpawnItem();
	StageManager::GetInstance().superStar.push_back(newSuperStar);
	RigidBody::collidables.push_back(newSuperStar);
}

//Axe button to clear the bridge
void CreateAxe(float x, float y)
{
	Axe* newAxe = new Axe();
	newAxe->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
	StageManager::GetInstance().axe.push_back(newAxe);
}

//Moving platform (動いてる足場）
void CreateMovingPlatform(float ax, float ay, float bx, float by)
{
	MovingPlatform* newMovingPlatform = new MovingPlatform();
	newMovingPlatform->Init(ax * BLOCK_SIZE, ay * BLOCK_SIZE, bx * BLOCK_SIZE, by * BLOCK_SIZE);
	StageManager::GetInstance().movingPlatform.push_back(newMovingPlatform);
}


vector<Collidable*> StageManager::GetCollidables()
{
	// returns all collidable blocks for mario to register

	vector<Collidable*> result;

	//ALL BLOCKS
	for (auto& g : ground)
		result.push_back(&g);
	for (auto* block : overworld1_1Blocks) {
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
	for (auto* block : overworld1_4Blocks) {
		if (block->IsActive()) {
			Collidable* physicsObject = dynamic_cast<Collidable*>(block);
			if (physicsObject != nullptr) {
				result.push_back(physicsObject);
			}
			// Unpack the spinning fireballs if this block is a FireBar
			FireBar* fireBar = dynamic_cast<FireBar*>(block);
			if (fireBar != nullptr) {
				for (auto& fireball : fireBar->GetFireballs()) {
					result.push_back(&fireball); // Safe conversion to Collidable*
				}
			}
		}
	}

	for (auto& g : underworld_ground)
		result.push_back(&g);
	for (auto* block : underworldBlocks) {
		if (block->IsActive()) {
			Collidable* physicsObject = dynamic_cast<Collidable*>(block);
			if (physicsObject != nullptr) {
				result.push_back(physicsObject);
			}
		}
	}

	for (auto* pipe : underWorldPipe) {
		if (pipe->IsActive()) {
			result.push_back(pipe); // Makes the pipe solid to Mario!
		}
	}

	for (auto* a : axe)
	{
		result.push_back(a);
	}

	for (auto* b : bridge)
	{
		result.push_back(b);
	}

	for (auto* m : movingPlatform)
	{
		result.push_back(m);
	}

	//ENEMIES
	for (auto* g : goomba)
	{
		result.push_back(g);
	}

	for (auto* k : koopaTroopa)
	{
		result.push_back(k);
	}
	for (auto* b : bowser)
	{
		result.push_back(b);
	}

	//ITEMS
	//coin
	for (auto* g : coins)
	{
		result.push_back(g);
	}

	//Super Mushroom
	for (auto* superM : superMushroom)
	{
		result.push_back(superM);
	}
	for (auto* fireF : fireFlower)
	{
		result.push_back(fireF);
	}
	for (auto* superS : superStar)
	{
		result.push_back(superS);
	}

	//ゴールポールを物理判定リストに追加
	for (auto* pole : goalPoles)
	{
		result.push_back(pole); // ① すり抜けるポール本体の判定
		result.push_back(&(pole->baseCollider)); // ② 固い土台の判定
	}

	// when u add pipe, questionblock etc just do:
	// for (auto& p : pipes)
	//     result.push_back(&p);
	return result;
}

void StageManager::Init(Stage stageNumber)
{
	isBridgeClearing = false;
	isBossDefeat = false;

	world1_1Handle = LoadGraph("data/image/Fonts/1_1.png");
	world1_4Handle = LoadGraph("data/image/Fonts/1_4.png");
	tex_brickParts = LoadGraph("data/image/brickpart.png");
	if (stageNumber == Stage::WORLD_1_1)
	{
		//Stage (ground1)
		CreateGrounds(0, 13, 68, 14); //from (0, 13) to (20, 14) | (0, 13)縺九ｉ(68, 14)縺ｾ縺ｧ
		CreateGrounds(71, 13, 85, 14);
		CreateGrounds(88, 13, 152, 14);
		CreateGrounds(155, 13, 210, 14);

		int uw_ground = LoadGraph("data/image/underworld_ground.png");
		int uw_brickblock = LoadGraph("data/image/underworld_brickblock.png");
		for (int x = 56; x <= 72; x++)
		{
			for (int y = 28; y <= 29; y++)
			{
				Ground g;
				g.Init(x * BLOCK_SIZE, y * BLOCK_SIZE, uw_ground);
				underworld_ground.push_back(g);
			}
		}

		//Stairs
		CreateStairs(134, 9, 137, 12, false);

		int texBrick = LoadGraph("data/image/brick_block.png");
		int texQuestion = LoadGraph("data/image/question_block.png");
		//int texQuestion = LoadGraph("data/image/QuestionBlockAnimation.png");
		int texEmpty = LoadGraph("data/image/empty_block.png");
		int texHard = LoadGraph("data/image/hard_block.png");
		int tLeft = LoadGraph("data/image/top_left.png");
		int tRight = LoadGraph("data/image/top_right.png");
		int bLeft = LoadGraph("data/image/bottom_left.png");
		int bRight = LoadGraph("data/image/bottom_right.png");

		int pHole = LoadGraph("data/image/underworld_pipe.png");
		int pLong = LoadGraph("data/image/underworld_lpipe.png");


		int totalOverworldBlocks = sizeof(world1_1data) / sizeof(BlockSpawnData);

		//Stairs
		CreateStairs(134, 9, 137, 12, false);

		for (int i = 0; i < totalOverworldBlocks; i++)
		{
			Float2 pixelPos;
			pixelPos.x = world1_1data[i].gridX * 64.0f;
			pixelPos.y = world1_1data[i].gridY * 64.0f;


			if (world1_1data[i].type == BlockType::BRICK)
			{
				BrickBlock* regularBrick = new BrickBlock();
				regularBrick->Init(pixelPos, texBrick);
				overworld1_1Blocks.push_back(regularBrick);
			}
			else if (world1_1data[i].type == BlockType::COINBRICK)
			{
				BrickBlock* coinBrick = new BrickBlock();
				coinBrick->Init(pixelPos, texBrick, texHard, BrickBlockItem::COIN, 9);
				overworld1_1Blocks.push_back(coinBrick);
			}
			else if (world1_1data[i].type == BlockType::STARBRICK)
			{
				BrickBlock* starBrick = new BrickBlock();
				starBrick->Init(pixelPos, texBrick, texHard, BrickBlockItem::STAR, 1);
				CreateSuperStar(pixelPos.x, pixelPos.y);
				overworld1_1Blocks.push_back(starBrick);
			}
			else if (world1_1data[i].type == BlockType::QUESTION)
			{
				QuestionBlockItem itemType = world1_1data[i].itemType;
				QuestionBlock* qblock = new QuestionBlock();
				qblock->Init(pixelPos, texQuestion, texHard, itemType);
				overworld1_1Blocks.push_back(qblock);
			}
			else if (world1_1data[i].type == BlockType::HIDDEN)
			{
				HiddenBlock* hiddenBlock = new HiddenBlock();
				hiddenBlock->Init(pixelPos, texHard, HiddenItemType::MUSHROOM_1UP);

				overworld1_1Blocks.push_back(hiddenBlock);
			}
			else if (world1_1data[i].type == BlockType::HARD)
			{
				HardBlock* hblock = new HardBlock();
				hblock->Init(pixelPos, texHard);
				overworld1_1Blocks.push_back(hblock);
			}
			else if (world1_1data[i].type == BlockType::PIPE)
			{
				Pipe* myPipe = new Pipe();
				myPipe->Init(pixelPos, tLeft, tRight, bLeft, bRight, world1_1data[i].pipeHeight, world1_1data[i].isWarpPipe);
				globalPipes.push_back(myPipe);
			}
		}

		int totalUnderworldBlocks = sizeof(underworld1_1data) / sizeof(BlockSpawnData);
		for (int i = 0; i < totalUnderworldBlocks; i++) {
			Float2 pos(underworld1_1data[i].gridX * 64.0f, underworld1_1data[i].gridY * 64.0f);
			/*if (underworld1_1data[i].type == BlockType::UNDERWORLDLPIPE) {
				UnderWorldLPipe* p = new UnderWorldLPipe();
				p->Init(pos, pHole, pLong, underworld1_1data[i].pipeHeight, underworld1_1data[i].isWarpPipe);
				underworldBlocks.push_back(p);
			}*/
			if (underworld1_1data[i].type == BlockType::UNDERWORLDPIPE) {
				UnderWorldPipe* p = new UnderWorldPipe();
				p->Init(pos, pHole);
				underworldBlocks.push_back(p);
			}
			else if (underworld1_1data[i].type == BlockType::HARD) {
				// Instantiate underworld hard tiles into underworldBlocks vector if needed...
				HardBlock* hard = new HardBlock();
				hard->Init(pos, uw_brickblock);
				underworldBlocks.push_back(hard);
			}
			else if (underworld1_1data[i].type == BlockType::HARDPIPE) {
				// Instantiate underworld hard tiles into underworldBlocks vector if needed...
				HardBlock* hard = new HardBlock();
				hard->Init(pos, pLong);
				underworldBlocks.push_back(hard);
			}
		}

		CreateStairs(140, 9, 143, 12, true);

		CreateStairs(148, 9, 152, 12, false);

		CreateStairs(155, 9, 158, 12, true);

		CreateStairs(181, 5, 189, 12, false);

		//ENEMIES
		//goomba
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

		//koopa
		CreateKoopaTroopa(106, 12);

		//COINS
		//underworld coins
		CreateCoin(61, 20, 65, 20);
		CreateCoin(60, 22, 66, 22);
		CreateCoin(60, 24, 66, 24);

		//BACKGROUND
		background.InitialImageAndSize(LoadGraph("data/image/1-1_background.png"));
		background.pos.Set(0.0f, 0.0f);

		// ゴールポールを X=12700 の位置に設置
		// ※ Y座標(192.0f)は仮です。goal_pole.pngの画像の高さに合わせて、
		// 床(Y=832)にぴったりくっつくように微調整してください。
		GoalPole* pole = new GoalPole();
		pole->Init(12670.0f, 160.0f);
		goalPoles.push_back(pole);

		// お城を設置
		Castle* myCastle = new Castle();
		myCastle->Init(12928.0f, 512.0f);
		castles.push_back(myCastle);
	}
	else if (stageNumber == Stage::WORLD_1_4)
	{
		
		//Reset mario 
		MainMario.position.Set(2 * BLOCK_SIZE, 5 * BLOCK_SIZE);

		//Reset Camera
		MainCamera.pos.Set(0.0f, 0.0f);
		MainCamera.cameraPosXLimit = 10240 - SCREEN_W;


		//Set background
		background.InitialImageAndSize(LoadGraph("data/image/1-4_background.png"));
		SetBackgroundColor(0, 0, 0);

		//Reset Stage
		int groundHandle = LoadGraph("data/image/ground_castle.png");
		int tmpHandle = LoadGraph("data/image/tmp.png");
		int bridgeHandle = LoadGraph("data/image/bridge.png");

		int fireSpriteHandle = LoadGraph("data/image/firebarAnimation.png");
		int blockSpriteHandle = LoadGraph("data/image/firebarblock.png");
		clearHandle = LoadGraph("data/image/clear_text.png");

		int totalOverworldBlocks = sizeof(world1_4data) / sizeof(BlockSpawnData);
		for (int i = 0; i < totalOverworldBlocks; i++)
		{
			Float2 pixelPos;
			pixelPos.x = world1_4data[i].gridX * 64.0f;
			pixelPos.y = world1_4data[i].gridY * 64.0f;
			if (world1_4data[i].type == BlockType::FIREBAR)
			{
				FireBar* fireBar = new FireBar();

				fireBar->Init(pixelPos, blockSpriteHandle, fireSpriteHandle, world1_4data[i].rotationDir, 6, world1_4data[i].startAngle);
				overworld1_4Blocks.push_back(fireBar);
			}
			else if (world1_4data[i].type == BlockType::HARD)
			{
				HardBlock* hblock = new HardBlock();
				hblock->Init(pixelPos, blockSpriteHandle);
				overworld1_4Blocks.push_back(hblock);
			}
			else if (world1_4data[i].type == BlockType::HIDDEN)
			{
				HiddenBlock* hiddenBlock = new HiddenBlock();
				hiddenBlock->Init(pixelPos, blockSpriteHandle, HiddenItemType::COIN);

				overworld1_4Blocks.push_back(hiddenBlock);
			}
		}
		//BOTTOM
		CreateGrounds(0, 7, 2, 14, groundHandle);
		CreateGrounds(3, 8, 3, 14, groundHandle);
		CreateGrounds(4, 9, 4, 14, groundHandle);
		CreateGrounds(5, 10, 12, 14, groundHandle);

		CreateGrounds(15, 10, 25, 14, groundHandle);

		CreateGrounds(29, 10, 29, 14, groundHandle);
		CreateGrounds(30, 11, 30, 14, groundHandle);
		CreateGrounds(31, 10, 31, 14, groundHandle);

		CreateGrounds(35, 9, 71, 14, groundHandle);
		CreateGrounds(72, 10, 103, 14, groundHandle);

		CreateGrounds(73, 13, 115, 14, groundHandle);

		CreateGrounds(116, 10, 119, 14, groundHandle);

		CreateGrounds(120, 13, 122, 14, groundHandle);

		CreateGrounds(123, 10, 127, 14, groundHandle);

		//bridge
		CreateBridge(128, 10, 140, 10, bridgeHandle);
		//axe
		CreateAxe(141, 8);
		//Moving Platform
		CreateMovingPlatform(137, 6, 139, 6);

		//block
		CreateGrounds(141, 9, 143, 14, groundHandle);
		CreateGrounds(144, 13, 159, 14, groundHandle);

		//TOP
		CreateGrounds(0, 2, 159, 2, groundHandle);

		CreateGrounds(0, 3, 23, 4, groundHandle);
		CreateGrounds(0, 2, 23, 4, groundHandle);
		CreateGrounds(23, 5, 23, 5, groundHandle); //one below it

		CreateGrounds(37, 3, 71, 5, groundHandle);

		CreateGrounds(80, 3, 80, 3, groundHandle); //two one block
		CreateGrounds(88, 3, 88, 3, groundHandle);

		CreateGrounds(97, 3, 103, 4, groundHandle);

		CreateGrounds(123, 3, 127, 4, groundHandle);

		//after the bridge
		CreateGrounds(142, 3, 143, 5, groundHandle);

		//ENEMIES
		CreateBowser(138, 8);
	}

}

void StageManager::Update(Camera& camera)
{



	if (currentzone == WorldZone::OVERWORLD) {
		for (IBlock* b : overworld1_1Blocks) { b->Update(); }
		for (IBlock* b : overworld1_4Blocks) { b->Update(); }
		for (Pipe* pipe : globalPipes)
		{
			pipe->Update();
		}
		for (int i = 0; i < goomba.size(); i++)
		{
			goomba[i]->Update(camera);
			if (MainMario.starEffect.isActive)
			{
				goomba[i]->isTrigger = true;
			}
			else
			{
				goomba[i]->isTrigger = false;
			}

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

		for (int i = 0; i < koopaTroopa.size(); i++)
		{
			koopaTroopa[i]->Update(camera, MainMario);


			if (MainMario.starEffect.isActive)
			{
				koopaTroopa[i]->isTrigger = true;
			}
			else
			{
				koopaTroopa[i]->isTrigger = false;
			}

			if (koopaTroopa[i]->state == EnemyState::DEAD)
			{
				// remove from collidables list too!
				RigidBody::collidables.erase(
					remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), koopaTroopa[i]),
					RigidBody::collidables.end()
				);
				koopaTroopa.erase(koopaTroopa.begin() + i);
			}
		}
	}
	else {
		for (IBlock* b : underworldBlocks) b->Update();
		for (UnderWorldPipe* pipe : underWorldPipe)
		{
			pipe->Update();
		}
	}

	for (int i = 0; i < bowser.size(); i++)
	{
		bowser[i]->Update(camera);

		//disable collision when mario in star mode
		if (MainMario.starEffect.isActive)
		{
			bowser[i]->isTrigger = true;
		}
		else
		{
			bowser[i]->isTrigger = false;
		}

		if (bowser[i]->state == EnemyState::DEAD)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), bowser[i]),
				RigidBody::collidables.end()
			);
			bowser.erase(bowser.begin() + i);

			if(axe.empty()) // すでに斧が取られて無くなっているなら
			{
				SoundManager::GetInstance().StopBGM();
				SoundManager::GetInstance().PlayBGMOnce("Stage1-4_Clear");

				float toadTargetX = 152 * BLOCK_SIZE;
				MainMario.StartCutsceneWalk(toadTargetX, 3.0f);
			}
		}
	}

	//Bowser fire's projectile
	for (int i = 0; i < bowserFire.size(); i++)
	{
		bowserFire[i]->Update();

		if (!bowserFire[i]->isActive)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), bowserFire[i]),
				RigidBody::collidables.end()
			);
			bowserFire.erase(bowserFire.begin() + i);
		}
	}
	for (auto* part : brickParts) // ここを auto* に
	{
		if (part != nullptr)
		{
			part->Update(); // .Update() から ->Update() に変更
		}
	}
	//Coin
	for (int i = 0; i < coins.size(); i++)
	{
		//Animation
		coins[i]->AnimationUpdate();
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

	//Deactive all blocks
	for (int i = 0; i < overworld1_1Blocks.size(); i++)
	{
		if (!overworld1_1Blocks[i]->IsActive())
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(),
					dynamic_cast<Collidable*>(overworld1_1Blocks[i])),  //cast here
				RigidBody::collidables.end()
			);
			//remove coins from stage manager
			overworld1_1Blocks.erase(overworld1_1Blocks.begin() + i);
		}
	}

	//Moving platform
	for (int i = 0; i < movingPlatform.size(); i++)
	{
		movingPlatform[i]->Update(MainMario);

		if (!movingPlatform[i]->isActive)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(),
					dynamic_cast<Collidable*>(movingPlatform[i])),  //cast here
				RigidBody::collidables.end()
			);
			//remove coins from stage manager
			movingPlatform.erase(movingPlatform.begin() + i);
		}
	}

	//deactivate axe
	for (int i = 0; i < axe.size(); i++)
	{
		if (!axe[i]->isActive)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(),
					dynamic_cast<Collidable*>(axe[i])),  //cast here
				RigidBody::collidables.end()
			);
			//remove coins from stage manager
			axe.erase(axe.begin() + i);
		}
	}

	//Super Mushroom
	for (int i = 0; i < superMushroom.size(); i++)
	{
		//Update for moving
		superMushroom[i]->Update();
		if (!superMushroom[i]->active)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), superMushroom[i]),
				RigidBody::collidables.end()
			);
			//remove coins from stage manager
			superMushroom.erase(superMushroom.begin() + i);
		}
	}
	//1up Mushroom
	for (int i = 0; i < upMushroom.size(); i++)
	{
		//Update for moving
		upMushroom[i]->Update();
		if (!upMushroom[i]->active)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), upMushroom[i]),
				RigidBody::collidables.end()
			);
			//remove coins from stage manager
			upMushroom.erase(upMushroom.begin() + i);
		}
	}

	//Fire flower
	for (int i = 0; i < fireFlower.size(); i++)
	{
		//Update for moving
		fireFlower[i]->Update();
		if (!fireFlower[i]->active)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), fireFlower[i]),
				RigidBody::collidables.end()
			);
			//remove coins from stage manager
			fireFlower.erase(fireFlower.begin() + i);
		}
	}

	//Super Star
	for (int i = 0; i < superStar.size(); i++)
	{
		//Update for moving
		superStar[i]->Update();
		if (!superStar[i]->active)
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), superStar[i]),
				RigidBody::collidables.end()
			);
			//remove coins from stage manager
			superStar.erase(superStar.begin() + i);
		}
	}

	// ゴールポール
	for (GoalPole* pole : goalPoles) { pole->Update(); }

	//Clear bridge in 1-4
	if (isBridgeClearing)
	{
		ClearBridge();
	}

	//camera limit in 1-4 until cutscene
	if (currentStage == Stage::WORLD_1_4)
	{
		PlayCutscene();

		
	}
}

void StageManager::Render(Camera& camera)
{
	//ENEMIES
	//Goomba
	for (int i = 0; i < goomba.size(); i++)
	{
		goomba[i]->RenderGlobal(camera);
	}

	for (int i = 0; i < koopaTroopa.size(); i++)
	{
		koopaTroopa[i]->RenderGlobal(camera);
	}

	//ITEMS
	for (Coin* coin : coins)
	{
		if (coin->active)
		{
			coin->RenderGlobal(camera);
		}
	}

	for (SuperMushroom* superM : superMushroom)
	{
		if (superM->active)
		{
			superM->RenderGlobal(camera);
		}
	}
	for (UpMushroom* superM : upMushroom)
	{
		if (superM->active)
		{
			superM->RenderGlobal(camera);
		}
	}
	for (FireFlower* fireF : fireFlower)
	{
		if (fireF->active)
		{
			fireF->RenderGlobal(camera);
		}
	}

	for (SuperStar* superS : superStar)
	{
		if (superS->active)
		{
			superS->RenderGlobal(camera);
		}
	}
	for (auto* part : brickParts) // ここを auto* に
	{
		// 念のためアクティブな場合のみ描画
		if (part != nullptr && part->IsActive())
		{
			part->Render(camera); // .Render() から ->Render() に変更
		}
	}

	for (Castle* c : castles)
	{
		c->RenderGlobal(camera);
	}

	for (auto* b : bowser)
	{
		b->RenderGlobal(camera, MainMario);
	}
	for (auto* bf : bowserFire)
	{
		bf->RenderGlobal(camera);
	}

	for (auto* a : axe)
	{
		a->RenderGlobal(camera);
	}

	for (auto* b : bridge)
	{
		b->RenderGlobal(camera);
	}

	for (auto* m : movingPlatform)
	{
		m->RenderGlobal(camera);
	}


	if (currentzone == WorldZone::OVERWORLD)
	{
		for (size_t i = 0; i < ground.size(); i++) ground[i].RenderGlobal(camera);
		for (IBlock* b : overworld1_1Blocks) b->Render(camera);
		for (IBlock* b : overworld1_4Blocks) b->Render(camera);
		for (Pipe* pipe : globalPipes)
		{
			pipe->Render(camera);
		}
	}
	else // RENDER UNDERWORLD ZONE
	{
		// Paint screen pitch black instead of light blue sky


		for (size_t i = 0; i < underworld_ground.size(); i++) underworld_ground[i].RenderGlobal(camera);
		for (IBlock* b : underworldBlocks) b->Render(camera);

		for (UnderWorldPipe* pipe : underWorldPipe)
		{
			pipe->Render(camera);
			camera.GlobalRenderBox(pipe->collider.x, pipe->collider.y, pipe->collider.x + pipe->collider.width, pipe->collider.y + pipe->collider.height, GetColor(255, 0, 0), FALSE);
		}
	}
	// ゴールポール
	for (GoalPole* pole : goalPoles) { pole->RenderGlobal(camera); }


	if (StageManager::GetInstance().currentStage == Stage::WORLD_1_4)
	{
		MarioFont::GetInstance().DrawWorldNumberLabel(SCREEN_W - 482, 52, world1_4Handle); // Render 1_4.png asset
	}
	else
	{
		MarioFont::GetInstance().DrawWorldNumberLabel(SCREEN_W - 482, 52, world1_1Handle); // Render 1_1.png asset
	}
	if (turnOnText)
	{
		timerShowText.Update();
		bool fullText = timerShowText.TimerHit();
		int offset = 0;
		if (fullText) offset = 2000;
		//set titled
		DrawRectGraph(0, 200, 0, 0, 1024, 300 + offset, clearHandle, 1);
	}

}

//Clear all objects in stage
void StageManager::ClearStage()
{
	// delete pointers first to avoid memory leak
	for (auto* g : goomba) delete g;
	for (auto* k : koopaTroopa) delete k;
	for (auto* b : overworld1_1Blocks) delete b;
	for (auto* b : underworldBlocks) delete b;
	for (auto* b : globalBlocks) delete b;
	for (auto* b : hardPipe) delete b;
	for (auto* p : globalPipes) delete p;
	for (auto* p : underWorldPipe) delete p;
	for (auto* c : coins) delete c;
	for (auto* m : superMushroom) delete m;
	for (auto* f : fireFlower) delete f;
	for (auto* s : superStar) delete s;
	for (auto* p : goalPoles) delete p;
	for (auto* c : castles) delete c;
	for (auto* b : bowser) delete b;
	for (auto* bf : bowserFire) delete bf;
	for (auto* a : axe) delete a;
	for (auto* b : bridge) delete b;
	for (auto* m : movingPlatform) delete m;
	// --- ここを追加して破片のメモリを解放する ---
	for (auto* part : brickParts)
	{
		delete part; // 動的生成したメモリを解放
	}

	// then clear the vectors
	ground.clear();
	goomba.clear();
	koopaTroopa.clear();
	overworld1_1Blocks.clear();
	underworldBlocks.clear();
	globalBlocks.clear();
	hardPipe.clear();
	globalPipes.clear();
	underWorldPipe.clear();
	coins.clear();
	superMushroom.clear();
	fireFlower.clear();
	superStar.clear();
	goalPoles.clear();
	castles.clear();
	bowser.clear();
	bowserFire.clear();
	axe.clear();
	bridge.clear();
	movingPlatform.clear();

	brickParts.clear(); // ベクターを空にする

	// clear collidables in RigidBody too!
	RigidBody::collidables.clear();
}

void StageManager::ClearBridge()
{
	if (isBridgeClearing && !bridge.empty())
	{
		timerClearingBridgeInterval.Update();

		if (timerClearingBridgeInterval.HitAndReset())
		{
			// remove from collidables list too!
			RigidBody::collidables.erase(
				remove(RigidBody::collidables.begin(), RigidBody::collidables.end(), bridge.back()),
				RigidBody::collidables.end()
			);

			delete bridge.back();
			bridge.pop_back();

			SoundManager::GetInstance().PlaySE("Bridge_Break");
		}

		if (bridge.empty())
		{
			isBridgeClearing = false; // done collapsing

			if (bowser.empty() && !SoundManager::GetInstance().IsPlayingBGM("Stage1-4_Clear"))
			{
				SoundManager::GetInstance().StopBGM();
				SoundManager::GetInstance().PlayBGMOnce("Stage1-4_Clear");

				float toadTargetX = 152 * BLOCK_SIZE;
				MainMario.StartCutsceneWalk(toadTargetX, 3.0f);
			}
		}
	}
}

void StageManager::PlayCutscene()
{
	if (MainMario.GetState() != MarioState::CUTSCENE) MainCamera.cameraPosXLimit = 126.5 * BLOCK_SIZE;
	else
	{

		if (MainCamera.cameraPosXLimit < 10240 - SCREEN_W && isBossDefeat) MainCamera.cameraPosXLimit += 5.0f;
		else if (MainCamera.cameraPosXLimit >= 10240 - SCREEN_W)
		{

			turnOnText = true;
			
		}
	}
}