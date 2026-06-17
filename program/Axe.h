#pragma once

#include "Collidable.h"
#include "HelpfulFunc.h"
#include "Camera.h"
class Axe : public Collidable
{
public:
	Axe() {
		isTrigger = true;            
	}

	//const
	static constexpr float SPEED = 3.0f;

	Float2 position;

	Animation spriteAnimation;
	bool isActive;

	//Method
	void Deactivate();
	void ClearBridge();

	//collider
	virtual void OnHitSide(RigidBody& mario) override;


	//Main thread
	void Init(float x, float y);
	void Update();
	void RenderGlobal(Camera& camera);
};