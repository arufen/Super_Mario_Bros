#pragma once

#include "RigidBody.h"
#include "Enemy.h"
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Hit.h"
#include  "CollectableItem.h"
#include "Mario.h"

class KoopaTroopa : public RigidBody, public Collidable, public Enemy
{
public:

	//const
	static constexpr float DEFAULT_SPEED = 1.0f;
	static constexpr float SHELL_SPEED = 6.0f;

	Animation spriteAnimation;
	Animation spriteAnimationShell;
	float direction;

	bool isInShell = false;
	bool isShellMoving = false;

	void ToDeadState();

	virtual void WaitForCamera(Camera& camera) override;
	virtual void TakeDamage(RigidBody& attacker) override;
	virtual void OnHitTop(RigidBody& player) override;
	virtual void OnHitSide(RigidBody& player) override;

	//Main thread
	void Init(float x, float y);
	void Update(Camera& camera, Mario& mario);
	void RenderGlobal(Camera& camera);


};