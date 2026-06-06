#include "Warp_Pipe.h"
#include "Mario.h"
#include "Dxlib.h"

Pipe::Pipe()
{
	pos.Set(0.0f, 0.0f);
	active = false;
	isWarpPipe = false;
	total_blocks = 2;
	texTL = -1;
	texTR = -1;
	texBL = -1;
	texBR = -1;
}

void Pipe::Init(Float2 startPos, int tLeft, int tRight, int bLeft, int bRight, int heightsInBlocks, bool warpEnable)
{
	pos = startPos;
	texTL = tLeft;
	texTR = tRight;
	texBL = bLeft;
	texBR = bRight;
	total_blocks = heightsInBlocks;
	isWarpPipe = warpEnable;
	active = true;

	float pipeWidth = BLOCK_SIZE * 2.0f;
	float pipeHeight = BLOCK_SIZE * (float)total_blocks;
	collider = Collider(pos.x, pos.y, pipeWidth, pipeHeight);
}

void Pipe::OnHitTop(RigidBody& player)
{
	if (!isWarpPipe)return;

	if (isWarpPipe && CheckHitKey(KEY_INPUT_S))
	{
		if (player.position.x > 3670.0f && player.position.x < 3702.0f)
		{
			Mario* mario = dynamic_cast<Mario*>(&player);
			if (mario != nullptr)
			{
				float pipeCenterX = this->pos.x + (BLOCK_SIZE);
				mario->position.x = pipeCenterX - ((float)mario->marioImage.sizeX / 2.0f);

				float surfaceY = this->pos.y - ((float)mario->marioImage.sizeX + 2.0f);
				mario->Warping(surfaceY);
			}
		}
	}
}

void Pipe::Update()
{

}

void Pipe::Render(Camera camera)
{
	if (!active) return;

	// Convert absolute world map positioning to localized viewport drawing coordinates
	extern Camera MainCamera;
	int screenX = (int)(pos.x - MainCamera.pos.x);
	int screenY = (int)(pos.y - MainCamera.pos.y);
	int sizeI = (int)BLOCK_SIZE;

	// 1. Draw the top pipe cap (Row 0)
	DrawGraph(screenX, screenY, texTL, TRUE);
	DrawGraph(screenX + sizeI, screenY, texTR, TRUE);

	// 2. Build the underlying body structure downwards
	for (int i = 1; i < total_blocks; i++)
	{
		int currentBodyRowY = screenY + (i * sizeI);

		DrawGraph(screenX, currentBodyRowY, texBL, TRUE);
		DrawGraph(screenX + sizeI, currentBodyRowY, texBR, TRUE);
	}
}