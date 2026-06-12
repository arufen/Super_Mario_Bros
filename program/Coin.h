#pragma once
#include "Float2.h"
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Collidable.h"

class Coin : public Collidable
{
public:
	Float2 position;
	/*Image image;*/
	Animation coinAnim;
	bool active = true; // To track if the coin is still active (not collected)
	
	void Init(float x, float y);
	void AnimationUpdate();
	void RenderGlobal(Camera& camera);
};