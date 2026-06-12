#include "Super_Star.h"


void SuperStar::CollectItem(Mario& mario)
{
	//TODO change mario state to big
	active = false;
}

void SuperStar::Init(float x, float y)
{
	int handle = LoadGraph("data/image/SuperStar.png");
	this->position.Set(x, y);
	this->spriteAnimation.InitialAnimation(handle, 4, 10);
	this->spriteAnimation.x = position.x;
	this->spriteAnimation.y = position.y;
	this->collider = Collider(x, y, (float)this->spriteAnimation.sprite.sizeX, (float)this->spriteAnimation.sprite.sizeY);
	this->RigidBody_collider = collider;
	this->direction = 1.0f;
	this->canCollect = false;
	this->originalPosition;
	this->active = true;
	this->now_speed_x = MOVE_SPEED;
	this->now_speed_y = 0.0f;
}

void SuperStar::Update()
{

	//Collider
	RigidBody_collider.x = position.x;
	RigidBody_collider.y = position.y;

	collider = RigidBody_collider;

	//image update
	spriteAnimation.x = position.x;
	spriteAnimation.y = position.y;

	//Update animation
	spriteAnimation.AnimationUpdateLoop();

	// shoot ray in front of goomba
	float checkX = (direction > 0)
		? RigidBody_collider.x + RigidBody_collider.width + 1
		: RigidBody_collider.x - 1;
	float checkY = RigidBody_collider.y + RigidBody_collider.height * 0.5f;

	Collidable* hit = Raycast(checkX, checkY);
	if (hit != nullptr)
	{
		//not colliding with enemy  & item
		Enemy* enemy = dynamic_cast<Enemy*>(hit);
		CollectableItem* item = dynamic_cast<CollectableItem*>(hit);
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

		//Check if its hit the ground
		float checkGround = RigidBody_collider.y + RigidBody_collider.height + 1.0f;
		Collidable* hit = Raycast(position.x + RigidBody_collider.width / 2.0f, checkGround);

		if (hit != nullptr && hit != dynamic_cast<Collidable*>(this))
		{
			//only jump on the ground
			Mario* mario = dynamic_cast<Mario*>(hit);
			Enemy* enemy = dynamic_cast<Enemy*>(hit);
			CollectableItem* collectableItem = dynamic_cast<CollectableItem*>(hit);
			if (!mario && !enemy && !collectableItem)
			{
				Jump();
			}

		}
	}

	//only moving up wehen it can be collected but not moving yet
	if (isSpawning && !canCollect)
	{
		MovingUp();
	}
}

void SuperStar::SpawnItem()
{
	if (!isSpawning)
	{
		originalPosition = position;
		isSpawning = true;
	}
}

void SuperStar::MovingUp()
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

void SuperStar::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderAnimation(spriteAnimation);
}

void SuperStar::Jump()
{
	now_speed_y += JUMP_SPEED;
}

//tmp
void SuperStar::OnHitSide(RigidBody& player)
{
	if (!canCollect && !isSpawning)  // only spawn if not already spawned or spawning
	SpawnItem();
}


