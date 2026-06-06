#include "Goomba.h"
#include "Ground.h"

void Goomba::Init(float x, float y, int handle)
{
	this->position.Set(x, y);
	this->image.pos = position;
	this->image.InitialImageAndSize(handle);
	this->collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);
	this->now_speed_y = 0.0f;
	this->now_speed_x = 1.0f;
	this->direction = 1.0f;
}


void Goomba::Update()
{

	now_speed_x = direction; // reset every frame FIRST

	//Collider
	collider.x = position.x;
	collider.y = position.y;

	//image update
	image.pos = position;

	//RigidBody update
	PhysicsUpdate();

	// shoot ray in front of goomba
	float checkX = (direction > 0)
		? collider.x + collider.width + 1
		: collider.x - 1;
	float checkY = collider.y + collider.height * 0.5f;

	Collidable* hit = Raycast(checkX, checkY);
	if (hit != nullptr)
	{
		direction *= -1.0f; // something in front flip!
	}

	// check if it's a specific type
	Ground* g = dynamic_cast<Ground*>(hit);
	if (g != nullptr)
	{

	}

	Goomba* otherGoomba = dynamic_cast<Goomba*>(hit);
	if (otherGoomba != nullptr && otherGoomba != this)
	{
		direction *= -1.0f;
	}

	if (now_speed_x == 0)
	{
		now_speed_x *= -1.0f;
	}

}

void Goomba::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderImage(image);
}
