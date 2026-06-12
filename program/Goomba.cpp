#include "Goomba.h"
#include "Ground.h"
#include "Mario.h"

void Goomba::Init(float x, float y, int handle)
{
	this->position.Set(x, y);
	this->image.pos = position;
	this->image.InitialImageAndSize(handle);
	this->RigidBody::collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);
	this->Collidable::collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);
	this->now_speed_y = 0.0f;
	this->now_speed_x = 1.0f;
	this->direction = 1.0f;
}


void Goomba::Update()
{

	now_speed_x = direction; // reset every frame FIRST

	//Collider
	RigidBody::collider.x = position.x;
	RigidBody::collider.y = position.y;

	Collidable::collider.x = position.x;
	Collidable::collider.y = position.y;

	//image update
	image.pos = position;

	//RigidBody update
	PhysicsUpdate();

	// shoot ray in front of goomba
	float checkX = (direction > 0)
		? RigidBody::collider.x + RigidBody::collider.width + 1
		: RigidBody::collider.x - 1;
	float checkY = RigidBody::collider.y + RigidBody::collider.height * 0.5f;

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

// ====================================================
// 衝突時の処理群
// ====================================================
void Goomba::OnHitTop(RigidBody& player)
{
	// マリオに上から踏まれたらクリボーは消滅（判定を無くす）
	RigidBody::collider.x = -9999.0f;
	RigidBody::collider.y = -9999.0f;
	RigidBody::collider.width = 0;
	RigidBody::collider.height = 0;

	Collidable::collider.x = -9999.0f;
	Collidable::collider.y = -9999.0f;
	Collidable::collider.width = 0;
	Collidable::collider.height = 0;

	// マリオをちょっと上にポコッと跳ね返らせる
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		mario->now_speed_y = -8.0f;
	}
}

void Goomba::OnHitSide(RigidBody& player)
{
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		mario->ToDeadState(); // 横から当たったら死亡処理へ
	}
}

void Goomba::OnHitBottom(RigidBody& player)
{
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		mario->ToDeadState(); // 下から当たっても死亡処理へ
	}
}
