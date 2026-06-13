#include "Question_Block.h"
#include "DxLib.h"

QuestionBlock::QuestionBlock()
{
	active = false;
	isBouncing = false;
	isAvailable = true;
	bounceTimer = 0.0f;
}

void QuestionBlock::Init(Float2 startPos, int active_graph, int empty_graph)
{
	pos = startPos;
	originalPos = startPos;
	texActive = active_graph;
	texEmpty = empty_graph;
	active = true;

	image.InitialImageAndSize(texActive);
	image.pos = pos;
	collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);
	itemType = QuestionBlockItem::COIN; // Default item type (can be set to other types as needed)

	if (itemType == QuestionBlockItem::COIN)
	{
		itemCoinAnim.InitialAnimation(LoadGraph("data/image/item_coin.png"), 4, 10);
		itemCoinAnim.x = pos.x + image.sizeX / 2.0f - itemCoinAnim.sprite.sizeX/ 2.0f;
		itemCoinAnim.y = pos.y + image.sizeY / 2.0f - itemCoinAnim.sprite.sizeY / 2.0f;
	}
}
//void QuestionBlock::Bump()
//{
//	
//}
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
	
}
void QuestionBlock::Update()
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
	if (itemCoinAnimationTimer.GetCurrentTimer() > 0.0f)
	{
		camera.GlobalRenderAnimation(itemCoinAnim);
	}

	if (!active) return;

	camera.GlobalRenderImage(image);

	
	
}