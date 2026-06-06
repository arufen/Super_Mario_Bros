#pragma once

#include "RigidBody.h"
#include "HelpfulFunc.h"
#include "Camera.h"



class Goomba : public RigidBody
{
public:
	Image image;
	float direction;

	//Main thread
	void Init(float x, float y, int handle);
	void Update();
	void RenderGlobal(Camera& camera);
};