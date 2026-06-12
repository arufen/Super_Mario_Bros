#pragma once


#include "CollectableItem.h"
#include "RigidBody.h"
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Float2.h"
class FireFlower : public CollectableItem
{
public:
	//variables
	Float2 position;
	Animation spriteAnimation;
	float direction;
	bool isSpawning;
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