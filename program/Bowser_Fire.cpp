#include "Bowser_Fire.h"
#include "StageManager.h"

void BowserFire::Init(float x, float y)
{
	int handle = LoadGraph("data/image/bowser_fire.png");
	this->position.Set(x, y);
	this->spriteAnimation.InitialAnimation(handle, 2, 3);
	this->RigidBody_collider = Collider(x, y, (float)this->spriteAnimation.sprite.sizeX, (float)this->spriteAnimation.sprite.sizeY);
	this->now_speed_y = 0.0f;
	this->now_speed_x = 1.0f;
	this->direction = -1.0f;
	this->layer = PhysicsLayer::ENEMY;
	this->direction = -1.0f;
	this->isActive = true;
	this->isTrigger = true;
}


void BowserFire::Update()
{
	now_speed_x = direction; // reset every frame FIRST

	//UPdate Collider
	RigidBody_collider.x = position.x;
	RigidBody_collider.y = position.y;

	collider = RigidBody_collider;

	//image update
	spriteAnimation.x = position.x;
	spriteAnimation.y = position.y;
	spriteAnimation.AnimationUpdateLoop();

	if (isActive)
	{
		position.x += direction * SPEED;
	}

	// shoot ray in front of goomba
	float checkX = (direction > 0)
		? RigidBody_collider.x + RigidBody_collider.width + 1
		: RigidBody_collider.x - 1;
	float checkY = RigidBody_collider.y + RigidBody_collider.height * 0.5f;

	Collidable* hit = Raycast(checkX, checkY);
	if (hit != nullptr)
	{
		Deactivate();
	}
}

void BowserFire::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderAnimation(spriteAnimation);
}


void BowserFire::Deactivate()
{
	isActive = false;
}

void BowserFire::OnHitSide(RigidBody& player)
{
	// すでに踏まれて死んでいる（潰れかけの）クリボーなら判定しない
	if (!isActive) return;

	// 当たってきたオブジェクトがマリオかどうかをチェック
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		// マリオを死亡状態にする
		if (mario->starEffect.isActive)
		{
			Deactivate();
		}
		else
		{
			mario->ToDeadState();
		}

	}
}
