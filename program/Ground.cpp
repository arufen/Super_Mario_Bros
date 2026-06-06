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


    //debug
	/*camera.GlobalRenderBox(collider.x, collider.y, (collider.x + collider.width), (collider.y + collider.height), GetColor(255, 0, 0), false);*/
	/*DrawCircleAA(position.x, position.y, 5, 10, GetColor(0, 255, 0), true);*/
}