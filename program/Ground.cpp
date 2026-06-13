#include "Ground.h"
#include "Main.h"
void Ground::Init(float x, float y, int handle)
{
	this->position.Set(x, y);
	this->image.pos = position;
	this->image.InitialImageAndSize(handle);
	this->collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);

}

void Ground::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderImage(image);
}