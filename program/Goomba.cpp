#include "Goomba.h"

void Goomba::Init(float x, float y, int handle)
{
	this->position.Set(x, y);
	this->image.pos = position;
	this->image.InitialImageAndSize(handle);
	this->collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);
}


void Goomba::Update()
{
	//test
	now_speed_x = 1.0f;

	//Collider
	collider.x = position.x;
	collider.y = position.y;

	//image update
	image.pos = position;

	//RigidBody update
	PhysicsUpdate();
}

void Goomba::GlobalRender(Camera& camera)
{
	camera.GlobalRenderImage(image);
}
