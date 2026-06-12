#pragma once

#include "CollectableItem.h"
#include "RigidBody.h"
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Enemy.h"
class SuperMushroom : public CollectableItem, public RigidBody
{
public:

	//const 
	static constexpr float MOVE_SPEED = 3.0f;
	//variables
	Image image;
	float direction;
	bool canMove;
	bool active; //if false, remove from the vector

	//CollectableItem function
	virtual void CollectItem(Mario& mario) override;
	virtual void SpawnItem() override;
	void MovingUp();

	//Main thread
	void Init(float x, float y);
	void Update();
	void RenderGlobal(Camera& camera);

	//tmp 
	virtual void OnHitSide(RigidBody& player) override;
};