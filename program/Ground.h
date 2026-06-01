#pragma once

#include "Float2.h"
#include "HelpfulFunc.h"
#include "Collider.h"
#include "Mario.h"
#include "Camera.h"
#include "Collidable.h"

class Ground : public Collidable
{
public:
	Float2 position;
	Image image;

	//Main thread
	void Init(float x, float y, int handle); //Set pos and image (座標とイメージを初期化)
	void Update(Mario& mario); //Check Collision ((当たり判定の時)
	void RenderGlobal(Camera& camera);
};