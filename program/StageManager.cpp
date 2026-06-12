#include "StageManager.h"

//Blocks
#include "Ground.h"

#include "Brick_Block.h"
#include "Question_Block.h"
#include "Hard_Block.h"
#include "Hidden_Block.h"

//Enemis
#include "Goomba.h"
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

		float spawnX = (164 * BLOCK_SIZE) - ((float)MainMario.marioImage.sizeX / 2.0f);

		float spawnY = 11 * BLOCK_SIZE + 32.0f;

		MainMario.now_speed_x = 0.0f;
		MainMario.now_speed_y = 0.0f;
		MainMario.isJumping = false;

		MainMario.Warping(spawnX, spawnY, Mario::WarpDir::UP);
	}
	else if (currentzone == WorldZone::UNDERWORLD)
	{
		MainMario.position.Set(57 * BLOCK_SIZE, 15 * BLOCK_SIZE);
		MainMario.RigidBody_collider.x = MainMario.position.x;
		MainMario.RigidBody_collider.y = MainMario.position.y;
		MainMario.now_speed_y = 0.0f;
	}
}

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
void CreateSuperMushroom(float x, float y) 
{
	SuperMushroom* newSuperMushroom = new SuperMushroom();
	newSuperMushroom->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
	StageManager::GetInstance().superMushroom.push_back(newSuperMushroom);
}

void CreateFireFlower(float x, float y)
{
	FireFlower* newFireFlower = new FireFlower();
	newFireFlower->Init(x * BLOCK_SIZE, y * BLOCK_SIZE);
	StageManager::GetInstance().fireFlower.push_back(newFireFlower);
}

vector<Collidable*> StageManager::GetCollidables()
{
	// returns all collidable blocks for mario to register
	
	vector<Collidable*> result;

	
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

	for (auto* g : goomba)
	{
		result.push_back(g);
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
	for (auto* fireF: fireFlower)
	{
		result.push_back(fireF);
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

	int texBrick =	LoadGraph("data/image/brick_block.png");
	int texQuestion = LoadGraph("data/image/question_block.png");
	int texEmpty =	LoadGraph("data/image/empty_block.png");
	int texHard = LoadGraph("data/image/hard_block.png");
	int tLeft =	LoadGraph("data/image/top_left.png");
	int tRight = LoadGraph("data/image/top_right.png");
	int bLeft =	LoadGraph("data/image/bottom_left.png");
	int bRight = LoadGraph("data/image/bottom_right.png");

	int pHole = LoadGraph("data/image/underworld_pipe.png");
	int pLong = LoadGraph("data/image/underworld_lpipe.png");

	int totalOVerworldBlocks = sizeof(world1_1data) / sizeof(BlockSpawnData);

	for (int i = 0; i < totalOVerworldBlocks; i++)
	{
		Float2 pixelPos;
		pixelPos.x = world1_1data[i].gridX * 64.0f;
		pixelPos.y = world1_1data[i].gridY * 64.0f;

		if (world1_1data[i].type == BlockType::BRICK)
		{
			BrickBlock* brick = new BrickBlock();
			brick->Init(pixelPos, texBrick);
			overworld1_1Blocks.push_back(brick);
		}
		else if (world1_1data[i].type == BlockType::QUESTION)
		{
			QuestionBlock* qblock = new QuestionBlock();
			qblock->Init(pixelPos, texQuestion, texHard);
			overworld1_1Blocks.push_back(qblock);
		}
		//else if (world1_1data[i].type == BlockType::HIDDEN)
		//{
		//	HiddenBlock* hiddenBlk = new HiddenBlock(pixelPos, texHard);
		//	overworld1_1Blocks.push_back(hiddenBlk);
		//}
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

			globalPipes.push_back(myPipe); // Save it to your vector!
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
	//underworld coins
	CreateCoin(61, 20, 65, 20); 
	CreateCoin(60, 22, 66, 22); 
	CreateCoin(60, 24, 66, 24); 

	//BACKGROUND
	background.InitialImageAndSize(LoadGraph("data/image/1-1_background.png"));
	background.pos.Set(0.0f, 0.0f);

	//tmp
	CreateSuperMushroom(5, 10);
	CreateFireFlower(6, 10);
}

void StageManager::Update(Camera& camera)
{
	if (currentzone == WorldZone::OVERWORLD) {
		for (IBlock* b : overworld1_1Blocks) { b->Update(); }
		for (Pipe* pipe : globalPipes)
		{
			pipe->Update();
		}
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
	}
	else {
		for (IBlock* b : underworldBlocks) b->Update();
		for (UnderWorldPipe* pipe : underWorldPipe)
		{
			pipe->Update();
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

}

void StageManager::Render(Camera& camera)
{
	if (currentzone == WorldZone::OVERWORLD)
	{
		for (size_t i = 0; i < ground.size(); i++) ground[i].RenderGlobal(camera);
		for (IBlock* b : overworld1_1Blocks) b->Render(camera);
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

	//ENEMIES
	//Goomba
	for (int i = 0; i < goomba.size(); i++)
	{
		goomba[i]->RenderGlobal(camera);
	}

	//ITEMS
	for (Coin* coin: coins)
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
	for (FireFlower* fireF : fireFlower)
	{
		if (fireF->active)
		{
			fireF->RenderGlobal(camera);
		}
	}

}