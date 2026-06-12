#pragma once

#include "RigidBody.h"
#include "Enemy.h"
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Hit.h"



class Goomba : public RigidBody, public Collidable, public Enemy
{
public:
	Image image;
	float direction;

	//Update
	void WaitForCamera(Camera& camera);
	virtual void OnHitTop(RigidBody& player) override;
	virtual void OnHitSide(RigidBody& player) override;
	virtual void OnHitBottom(RigidBody& player) override;

	void Destroy();

	//Main thread
	void Init(float x, float y, int handle);
	void Update(Camera& camera);
	void RenderGlobal(Camera& camera);

	
};