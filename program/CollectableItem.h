#pragma once
#include "Collidable.h"

class Mario;

class CollectableItem : public Collidable
{
public:
	static constexpr float SPAWN_SPEED = 2.0f;
	CollectableItem()
	{
		isTrigger = true;
		canCollect = false;
		layer = PhysicsLayer::ITEM;
	}

	bool canCollect;
	Float2 originalPosition;
	
	//When collected, make mario do something
	virtual void CollectItem(Mario& mario) {}
	virtual void SpawnItem() {}
};