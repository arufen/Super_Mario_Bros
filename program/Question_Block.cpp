#include "Question_Block.h"
#include "DxLib.h"
#include "StageManager.h"

QuestionBlock::QuestionBlock()
{
	active = false;
	isBouncing = false;
	isAvailable = true;
	bounceTimer = 0.0f;
}

void QuestionBlock::Init(Float2 startPos, int active_graph, int empty_graph, QuestionBlockItem itemType)
{
	pos = startPos;
	originalPos = startPos;
	texActive = active_graph;
	texEmpty = empty_graph;
	active = true;

	itemCoinAnim.InitialAnimation(LoadGraph("data/image/item_coin.png"), 4, 10);
	image.InitialImageAndSize(texActive);
	image.pos = pos;
	collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);
	this->itemType = itemType; // Default item type is coin

	if (itemType == QuestionBlockItem::COIN)
	{
		/*itemCoinAnim.InitialAnimation(LoadGraph("data/image/item_coin.png"), 4, 10);*/
		itemCoinAnim.x = pos.x + image.sizeX / 2.0f - itemCoinAnim.sprite.sizeX/ 2.0f;
		itemCoinAnim.y = pos.y + image.sizeY / 2.0f - itemCoinAnim.sprite.sizeY / 2.0f;
	}
}

void QuestionBlock::OnHitBottom(RigidBody& player)
{
	// Trigger the jump animation!
	if (!isAvailable || isBouncing) return;

	//ignore object other than mario
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (!mario) return;

	isBouncing = true;
	bounceTimer = 0.0f;
	itemCoinAnimationTimer.ResetTimer();

	isAvailable = false;

	image.InitialImageAndSize(texEmpty);

	// TODO: Spawn a coin or powerup item here!
	switch (itemType)
	{
	case QuestionBlockItem::COIN:
		mario->AddCoin();
		break;
	case QuestionBlockItem::POWER_UP:
		if(mario->GetForm() == MarioForm::SMALL)
		StageManager::GetInstance().CreateSuperMushroom(pos.x / BLOCK_SIZE, pos.y / BLOCK_SIZE);
		else
		StageManager::GetInstance().CreateFireFlower(pos.x / BLOCK_SIZE, pos.y / BLOCK_SIZE);
		break;
	}
	
}
void QuestionBlock::Update()
{
	if (itemType == QuestionBlockItem::COIN)
	{
		if (itemCoinAnimationTimer.GetCurrentTimer() > 0.0f && !isAvailable)
		{
			itemCoinAnimationTimer.Update();
			itemCoinAnim.AnimationUpdateLoop();
			float totalTime = itemCoinAnimationTimer.MAX_TIMER; // match this to your timer's MAX_TIMER
			float elapsed = totalTime - itemCoinAnimationTimer.GetCurrentTimer();
			float t = elapsed / totalTime; // goes 0.0 Å® 1.0
			float offset = -16.0f; // how much to offset the coin's starting position (pixels)
			float length = 64.0f * 3.5; // how high the coin goes (pixels)
			itemCoinAnim.y = pos.y + offset - sinf(t * 3.14) * length;
		}
	}

	if (!active)return;

	if (isBouncing)
	{
		bounceTimer += 1.0f;

		// Fast manual arcade bounce (Up for 6 frames, down for 6 frames)
		if (bounceTimer <= 6.0f)
		{
			pos.y -= 3.0f;
		}
		else if (bounceTimer <= 12.0f)
		{
			pos.y += 3.0f;
		}
		else
		{
			pos.y = originalPos.y; // Pin it back to original positioning exactly
			isBouncing = false;
			isAvailable = false;

		
		}
	}
	image.pos = pos;
	collider.y = pos.y;
	collider.x = pos.x;
}

void QuestionBlock::Render(Camera camera)
{
	//COIN
	if (itemType == QuestionBlockItem::COIN)
	{
		if (itemCoinAnimationTimer.GetCurrentTimer() > 0.0f)
		{
			camera.GlobalRenderAnimation(itemCoinAnim);
		}
	}

	if (!active) return;

	camera.GlobalRenderImage(image);

	
	
}