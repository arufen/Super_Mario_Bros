#include "Coin.h"

void Coin::Init(float x, float y)
{
	int handle = LoadGraph("data/image/coinAnimation.png");
	this->position.Set(x, y);
	/*this->image.pos = position;
	this->image.InitialImageAndSize(handle);*/
	this->coinAnim.InitialAnimation(handle, 6, 10);
	this->coinAnim.x = position.x;
	this->coinAnim.y = position.y;
	this->collider = Collider(x, y, (float)this->coinAnim.sprite.sizeX, (float)this->coinAnim.sprite.sizeY);
	this->isTrigger = true; // Coin is a trigger, not a solid block
	this->active = true;
}

void Coin::AnimationUpdate()
{
	if (active) {
		coinAnim.AnimationUpdateLoop();
	}
}

void Coin::RenderGlobal(Camera& camera)
{
	/*camera.GlobalRenderImage(image);*/
	camera.GlobalRenderAnimation(coinAnim);
}
