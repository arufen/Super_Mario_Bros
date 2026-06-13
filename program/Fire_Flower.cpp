#include "Fire_Flower.h"
#include "Mario.h"


void FireFlower::CollectItem(Mario& mario)
{
	mario.ChangeToFire();
	active = false;
}

void FireFlower::Init(float x, float y)
{
	int handle = LoadGraph("data/image/FireFlower.png");
	this->position.Set(x, y);
	this->spriteAnimation.InitialAnimation(handle, 4, 10);
	this->spriteAnimation.x = position.x;
	this->spriteAnimation.y = position.y;
	this->collider = Collider(x, y, (float)this->spriteAnimation.sprite.sizeX, (float)this->spriteAnimation.sprite.sizeY);
	this->direction = 1.0f;
	this->canCollect = false;
	this->originalPosition;
	this->active = true;
	this->canCollect = false;
	this->isSpawning = false;
}

void FireFlower::Update()
{
	collider.x = position.x;
	collider.y = position.y;

	//image update
	spriteAnimation.x = position.x;
	spriteAnimation.y = position.y;

	spriteAnimation.AnimationUpdateLoop();

	//only moving up wehen it can be collected but not moving yet
	if (isSpawning && !canCollect)
	{
		MovingUp();
	}
}

void FireFlower::SpawnItem()
{
	if (!isSpawning)
	{
		originalPosition = position;
		isSpawning = true;
	}
}

void FireFlower::MovingUp()
{
	//item moving up from question block
	float targetY = originalPosition.y - BLOCK_SIZE;

	if (position.y > targetY)
	{
		position.y -= CollectableItem::SPAWN_SPEED;
	}
	else
	{
		//Physics update
		isSpawning = false;
		canCollect = true;
	}

}

void FireFlower::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderAnimation(spriteAnimation);
}

//tmp
void FireFlower::OnHitSide(RigidBody& player)
{
	SpawnItem();
}
