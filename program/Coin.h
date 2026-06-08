#pragma once
#include "Float2.h"
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Collidable.h"

class Coin : public Collidable
{
public:
	Float2 position;
	Image image;
	bool active = true; // To track if the coin is still active (not collected)
	
	void Init(float x, float y, int handle);
	void RenderGlobal(Camera& camera);
};