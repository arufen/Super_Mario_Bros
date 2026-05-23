#include "Brick_Block.h"
#include "Dxlib.h"
#define MAX_BRICKBLOCKS 30
//BrickBlock Brick_block[MAX_BRICKBLOCKS];

void BrickBlock::Init()
{
	pos.x = 0;
	pos.y = 0;
	active = true;
}

void BrickBlock::Set(float x, float y)
{
	pos.x = x;
	pos.y = y;
}

void BrickBlock::Update()
{
	if (active)
	{

	}
}

void BrickBlock::Render()
{
	DrawGraph(pos.x, pos.y, image, TRUE);
}