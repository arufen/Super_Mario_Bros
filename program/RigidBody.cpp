#include "RigidBody.h"

vector<Collidable*> RigidBody::collidables;


Collidable* RigidBody::Raycast(float x, float y)
{
	for (auto* block : collidables)
	{
		if (x >= block->collider.x &&
			x <= block->collider.x + block->collider.width &&
			y >= block->collider.y &&
			y <= block->collider.y + block->collider.height)
		{
			return block; // hit something!
		}
	}
	return nullptr; // hit nothing
}

void RigidBody::ResolveCollision(Collidable& block)
{
	if (!collider.intersects(block.collider)) return;


	if (block.isTrigger)
	{
		// just call the hit callbacks, no pushing
		block.OnHitSide(*this);
		return;
	}

	float overlapLeft = (collider.x + collider.width) - block.collider.x;
	float overlapRight = (block.collider.x + block.collider.width) - collider.x;
	float overlapTop = (collider.y + collider.height) - block.collider.y;
	float overlapBottom = (block.collider.y + block.collider.height) - collider.y;

	float minX = min(overlapLeft, overlapRight);
	float minY = min(overlapTop, overlapBottom);

	const float bias = 0.0f;

	if (minY < minX + bias)
	{
		if (overlapTop < overlapBottom)
		{
			position.y -= overlapTop;
			collider.y -= overlapTop;
			now_speed_y = 0;
			block.OnHitTop(*this);
		}
		else
		{
			position.y += overlapBottom;
			collider.y += overlapBottom;
			now_speed_y = 0;
			block.OnHitBottom(*this);
		}
	}
	else
	{
		if (overlapLeft < overlapRight)
		{
			position.x -= overlapLeft;
			collider.x -= overlapLeft;
		}
		else
		{
			position.x += overlapRight;
			collider.x += overlapRight;
		}
		now_speed_x = 0;
		block.OnHitSide(*this);
	}
}

void RigidBody::PhysicsUpdate()
{
	// x movement here
	position.x += now_speed_x;

	collider.x = position.x;
	int steps = (int)(abs(now_speed_y) / (collider.height * 0.5f)) + 1;
	steps = min(steps, 10);

	for (int i = 0; i < steps; i++)
	{
		now_speed_y += GRAVITY / steps;
		position.y += now_speed_y / steps;
		collider.y = position.y;

		for (auto* block : collidables)
		{
			ResolveCollision(*block);
		}
	}
}