#include "Goomba.h"

void Goomba::Init(float x, float y, int handle)
{
	this->position.Set(x, y);
	this->image.pos = position;
	this->image.InitialImageAndSize(handle);
	this->RigidBody_collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);
	this->now_speed_y = 0.0f;
	this->now_speed_x = 1.0f;
	this->direction = -1.0f;
}


void Goomba::Update(Camera& camera)
{
	now_speed_x = direction; // reset every frame FIRST

	//Collider
	RigidBody_collider.x = position.x;
	RigidBody_collider.y = position.y;

	collider = RigidBody_collider;

	//image update
	image.pos = position;

	WaitForCamera(camera);

	if (state == EnemyState::ACTIVE)
	{	//RigidBody update
		PhysicsUpdate();
	}

	// shoot ray in front of goomba
	float checkX = (direction > 0)
		? RigidBody_collider.x + RigidBody_collider.width + 1
		: RigidBody_collider.x - 1;
	float checkY = RigidBody_collider.y + RigidBody_collider.height * 0.5f;

	Collidable* hit = Raycast(checkX, checkY);
	if (hit != nullptr)
	{
		direction *= -1.0f; 
	}

	now_speed_x = direction; // add this line!

	if (state == EnemyState::DEAD)
	{

	}
}

void Goomba::WaitForCamera(Camera& camera)
{
	Float2 cameraSize{ SCREEN_W, SCREEN_H };
	Float2 enemySize{ (float)image.sizeX, (float)image.sizeY };
	if (CheckBoxHit(camera.pos, cameraSize, position, enemySize))
	{
		if (state == EnemyState::WAITING)
		{
			state = EnemyState::ACTIVE;
		}
			
	}
}

void Goomba::OnHitTop(RigidBody& player)
{
	state = EnemyState::DEAD;
}

void Goomba::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderImage(image);
}
