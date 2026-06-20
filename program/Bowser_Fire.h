#pragma once
#include "RigidBody.h"
#include "Collidable.h"
#include "HelpfulFunc.h"
#include "Camera.h"
class BowserFire : public Collidable, public RigidBody
{
public:
	BowserFire() {
		isTrigger = true;            // Allow Mario to overlap cleanly without getting physically blocked
		layer = PhysicsLayer::ENEMY; // Tag it properly for your physics layers
	}

	//const
	static constexpr float SPEED = 3.0f;

	Animation spriteAnimation;
	float direction;
	bool isActive;
	
	bool hasPlayedSE = false;

	//Method
	void Deactivate();

	//collider
	virtual void OnHitSide(RigidBody& mario) override;


	//Main thread
	void Init(float x, float y);
	void Update();
	void RenderGlobal(Camera& camera);
};