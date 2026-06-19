#include "Super_Mushroom.h"
#include "Mario.h"


void SuperMushroom::CollectItem(Mario& mario)
{
	mario.ChangeToSuper();
	active = false;
}

void SuperMushroom::Init(float x, float y)
{
	int handle = LoadGraph("data/image/SuperMushroom.png");
	this->position.Set(x, y);
	this->image.pos = position;
	this->image.InitialImageAndSize(handle);
	this->collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);
	this->RigidBody_collider = collider;
	this->direction = -1.0f;
	this->canCollect = false;
	this->originalPosition;
	this->active = true;
	this->isSpawning = false;
	this->now_speed_x = MOVE_SPEED;
	this->now_speed_y = 0.0f;
}

void SuperMushroom::Update()
{

	//Collider
	RigidBody_collider.x = position.x;
	RigidBody_collider.y = position.y;

	collider = RigidBody_collider;

	//image update
	image.pos = position;

	// shoot ray in front of goomba
	float checkX = (direction > 0)
		? RigidBody_collider.x + RigidBody_collider.width + 1
		: RigidBody_collider.x - 1;
	float checkY = RigidBody_collider.y + RigidBody_collider.height * 0.5f;

	Collidable* hit = Raycast(checkX, checkY);
	if (hit != nullptr)
	{
		//not colliding with enemy  & item
		Enemy* enemy= dynamic_cast<Enemy*>(hit);
		CollectableItem* item= dynamic_cast<CollectableItem*>(hit);
		if (enemy == nullptr && item == nullptr)
		{
			direction *= -1.0f;
		}
			
	}

	now_speed_x = direction * MOVE_SPEED; // reset every frame FIRST

	if (!isSpawning && canCollect)
	{
		//Move the item
		PhysicsUpdate();
	}

	//only moving up wehen it can be collected but not moving yet
	if (isSpawning && !canCollect)
	{
		MovingUp();
	}
}

void SuperMushroom::SpawnItem()
{
	if (!isSpawning)
	{
		originalPosition = position;
		isSpawning = true;
	}
}

void SuperMushroom::MovingUp()
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


void SuperMushroom::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderImage(image);
}

////tmp
//void SuperMushroom::OnHitSide(RigidBody& player)
//{
//	SpawnItem();
//}
