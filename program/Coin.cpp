#include "Coin.h"

void Coin::Init(float x, float y, int handle)
{
	this->position.Set(x, y);
	this->image.pos = position;
	this->image.InitialImageAndSize(handle);
	this->collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);
	this->isTrigger = true; // Coin is a trigger, not a solid block
	this->active = true;
}

void Coin::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderImage(image);
}
