#pragma once

#include "RigidBody.h"
#include "Enemy.h"
#include "HelpfulFunc.h"
#include "Random.h"
class Mario;
class Bowser : public RigidBody, public Enemy, public Collidable
{
public:
	static constexpr int MAX_HP = 5;

	int HP; //when get hit by fire
	Animation spriteAnimation;

	float direction;
	Timer timerJump{ 4.0f };
	Timer timerShoot{ 5.0f };
	Timer timerisShooting{ 1.0f };
	bool isLookRight;

	//Update
	virtual void WaitForCamera(Camera& camera) override;
	virtual void TakeDamage(RigidBody& attacker) override;
	virtual  void OnHitSide(RigidBody& player) override;
	virtual  void OnHitTop(RigidBody& player) override;

	void Jump();
	void ShootProjectile();
	void Kill();

	//Main thread
	void Init(float x, float y);
	void Update(Camera& camera);
	void RenderGlobal(Camera& camera, const Mario& mario);

};