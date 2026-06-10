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
	if (!RigidBody_collider.intersects(block.collider)) return;


	if (block.isTrigger)
	{
		// just call the hit callbacks, no pushing
		block.OnHitSide(*this);
		return;
	}

	float overlapLeft = (RigidBody_collider.x + RigidBody_collider.width) - block.collider.x;
	float overlapRight = (block.collider.x + block.collider.width) - RigidBody_collider.x;
	float overlapTop = (RigidBody_collider.y + RigidBody_collider.height) - block.collider.y;
	float overlapBottom = (block.collider.y + block.collider.height) - RigidBody_collider.y;

	float minX = min(overlapLeft, overlapRight);
	float minY = min(overlapTop, overlapBottom);

	const float bias = 20.0f;

	if (minY < minX + bias)
	{
		if (overlapTop < overlapBottom)
		{
			position.y -= overlapTop;
			RigidBody_collider.y -= overlapTop;
			now_speed_y = 0;
			block.OnHitTop(*this);
		}
		else
		{
			position.y += overlapBottom;
			RigidBody_collider.y += overlapBottom;
			now_speed_y = 0;
			block.OnHitBottom(*this);
		}
	}
	else
	{
		if (overlapLeft < overlapRight)
		{
			position.x -= overlapLeft;
			RigidBody_collider.x -= overlapLeft;
		}
		else
		{
			position.x += overlapRight;
			RigidBody_collider.x += overlapRight;
		}
		now_speed_x = 0;
		block.OnHitSide(*this);
	}
}

void RigidBody::PhysicsUpdate()
{
	// x movement here
	position.x += now_speed_x;

	now_speed_y = min(now_speed_y, 15.0f);

	int steps = (int)(abs(now_speed_y) / (RigidBody_collider.height * 0.5f)) + 1;
	steps = min(steps, 10);

	for (int i = 0; i < steps; i++)
	{
		now_speed_y += GRAVITY / steps;
		position.y += now_speed_y / steps;
		RigidBody_collider.y = position.y;

		hitVertical = false;  // add this

		for (auto* block : collidables)
		{
			if (block == dynamic_cast<Collidable*>(this)) continue; //skip itself
			ResolveCollision(*block);
		}
		if (hitVertical) break;  // just this one line
	}

}