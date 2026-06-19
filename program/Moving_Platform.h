#pragma once
#pragma once

#include "Collidable.h"
#include "Enemy.h"
#include "HelpfulFunc.h"
#include "Camera.h"

class Mario;

class MovingPlatform :public Collidable
{
public:
	Image image;
	Float2 position;
	float direction;

	//For lerp movement between 2 points
	Float2 pointA;
	Float2 pointB;
	float t = 0.0f;        // progress between A and B, 0 = at A, 1 = at B
	float lerpSpeed = 0.01f; // how fast t changes per frame, tweak for slower/faster platform

	bool isActive;

	float now_speed_x; // how much the platform actually moved THIS frame (used to carry Mario along)

	//For mario able to move while on platform
	virtual void OnHitTop(RigidBody& player) override;
	void Deactivate();

	//Main thread
	void Init(float ax, float ay, float bx, float by);
	void Update(const Mario& mario);
	void RenderGlobal(Camera& camera);
};