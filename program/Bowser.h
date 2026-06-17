#pragma once

#include "RigidBody.h"
#include "Enemy.h"
#include "HelpfulFunc.h"

class Bowser : public RigidBody, public Enemy, public Collidable
{
public:
	static constexpr int MAX_HP = 5;

	int HP; //when get hit by fire
	Animation spriteAnimation;
	float direction;
	//Timer jumpTimer;

	//Update
	virtual void WaitForCamera(Camera& camera) override;
	virtual void TakeDamage(RigidBody& attacker) override;
	virtual  void OnHitSide(RigidBody& player) override;

	//Main thread
	void Init(float x, float y);
	void Update(Camera& camera);
	void RenderGlobal(Camera& camera);

};