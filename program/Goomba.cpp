#include "Goomba.h"
#include "Mario.h"

void Goomba::Init(float x, float y, int handle)
{
	this->position.Set(x, y);
	//not in shell sprite
	this->spriteAnimation.InitialAnimation(handle, 2, 6);
	this->spriteAnimation.x = position.x;
	this->spriteAnimation.y = position.y;
	this->RigidBody_collider = Collider(x, y, (float)this->spriteAnimation.sprite.sizeX, (float)this->spriteAnimation.sprite.sizeY);
	this->now_speed_y = 0.0f;
	this->now_speed_x = 1.0f;
	this->direction = -1.0f;
	this->layer = PhysicsLayer::ENEMY;
	//this->isTrigger = true;
}


void Goomba::Update(Camera& camera)
{
	now_speed_x = direction; // reset every frame FIRST

	//Collider
	RigidBody_collider.x = position.x;
	RigidBody_collider.y = position.y;

	collider = RigidBody_collider;

	//image update
	spriteAnimation.x = position.x;
	spriteAnimation.y = position.y;
	spriteAnimation.AnimationUpdateLoop();

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
		//not colliding with collectable item
		CollectableItem* item = dynamic_cast<CollectableItem*>(hit);
		if (item == nullptr)
		{
			direction *= -1.0f;
		}
	
	}

	now_speed_x = direction; // add this line!

	if (state == EnemyState::DEAD)
	{

	}
}

void Goomba::WaitForCamera(Camera& camera)
{
	Float2 cameraSize{ SCREEN_W, SCREEN_H };
	Float2 enemySize{ (float)spriteAnimation.sprite.sizeX, (float)spriteAnimation.sprite.sizeY };
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
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		mario->Jump();
		TakeDamage(player);
	}
}

// 横からマリオがぶつかった時の処理
void Goomba::OnHitSide(RigidBody& player)
{
	// すでに踏まれて死んでいる（潰れかけの）クリボーなら判定しない
	if (state == EnemyState::DEAD) return;

	// 当たってきたオブジェクトがマリオかどうかをチェック
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		// マリオを死亡状態にする
		if (mario->starEffect.isActive)
		{
			TakeDamage(player);
		}
		else
		{
			mario->ToDeadState();
		}
	
	}
}

// 下からマリオがぶつかった時の処理（ジャンプ中に頭をぶつけるなど）
void Goomba::OnHitBottom(RigidBody& player)
{
	if (state == EnemyState::DEAD) return;

	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		mario->ToDeadState();
	}
}

void Goomba::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderAnimation(spriteAnimation);
}

void Goomba::TakeDamage(RigidBody& attacker) 
{
	state = EnemyState::DEAD;
}