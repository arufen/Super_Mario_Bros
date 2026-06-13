#pragma once

#include "CollectableItem.h"
#include "RigidBody.h"
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Enemy.h"
class SuperStar : public CollectableItem, public RigidBody
{
public:

	//const 
	static constexpr float MOVE_SPEED = 5.0f;
	static constexpr float JUMP_SPEED = -16.0f;

	//variables
	Animation spriteAnimation;
	float direction;
	bool isSpawning;
	bool active; //if false, remove from the vector

	//CollectableItem function
	virtual void CollectItem(Mario& mario) override;
	virtual void SpawnItem() override;
	void MovingUp();
	void Jump();

	//Main thread
	void Init(float x, float y);
	void Update();
	void RenderGlobal(Camera& camera);

	//tmp 
	virtual void OnHitSide(RigidBody& player) override;
};