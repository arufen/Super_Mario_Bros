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
		MainMario.collider.x = MainMario.position.x;
		MainMario.collider.y = MainMario.position.y;
		MainMario.now_speed_y = 0.0f;
	}
}
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
	/*for (auto& g : brick_block)
		result.push_back(&g);*/
	
	
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
		else if (world1_1data[i].type == BlockType::HIDDEN)
		{
			HiddenBlock* hiddenBlk = new HiddenBlock(pixelPos, texHard);
			overworld1_1Blocks.push_back(hiddenBlk);
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

			globalPipes.push_back(myPipe); // Save it to your vector!
		}
		//underworld1_1data
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
	
}

void StageManager::Update()
{
	if (currentzone == WorldZone::OVERWORLD) {
		for (IBlock* b : overworld1_1Blocks) { b->Update(); }
		for (Pipe* pipe : globalPipes)
		{
			pipe->Update();
		}
		for (int i = 0; i < goomba.size(); i++)
		{
			goomba[i].Update();
		}
	}
	else {
		for (IBlock* b : underworldBlocks) b->Update();
		for (UnderWorldPipe* pipe : underWorldPipe)
		{
			pipe->Update();
		}
	}
	//Goomba
	/*for (int i = 0; i < goomba.size(); i++)
	{
		goomba[i].Update();
	}*/
	/*for (IBlock* block : overworld1_1Blocks)
	{
		block->Update();
	}
	for (Pipe* pipe : globalPipes)
	{
		pipe->Update();
	}*/
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
	//Stage
	/*for (int i = 0; i < ground.size(); i++)
	{
		ground[i].RenderGlobal(camera);
	}*/
	DrawFormatString(0, 48, GetColor(255, 255, 255),
		"Mario X: %.0f", MainMario.position.x);
	DrawFormatString(0, 64, GetColor(255, 255, 255),
		"Mario Y: %.0f", MainMario.position.y);
	//Goomba
	for (int i = 0; i < goomba.size(); i++)
	{
		goomba[i].RenderGlobal(camera);

		//DrawFormatString(100, 100 + 20 * i, GetColor(255, 255, 255), "speed: %f", goomba[i].now_speed_x);
	}
	/*for (IBlock* block : overworld1_1Blocks)
	{
		block->Render(camera);
	}
	for (Pipe* pipe : globalPipes)
	{
		pipe->Render(camera);
	}*/

}