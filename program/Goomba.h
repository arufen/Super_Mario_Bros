#pragma once

#include "RigidBody.h"
#include "Enemy.h"
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Hit.h"
#include  "CollectableItem.h"



class Goomba : public RigidBody, public Collidable, public Enemy
{
public:
	/*Image image;*/ //change to animation
	Animation spriteAnimation;
	float direction;

	//Update
	virtual void WaitForCamera(Camera& camera) override;
	virtual void TakeDamage(RigidBody& attacker) override;
	virtual void OnHitTop(RigidBody& player) override;
	virtual void OnHitSide(RigidBody& player) override;
	virtual void OnHitBottom(RigidBody& player) override;

	//Main thread
	void Init(float x, float y, int handle);
	void Update(Camera& camera);
	void RenderGlobal(Camera& camera);

	
};