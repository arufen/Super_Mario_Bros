#include "Bowser.h"
#include "Mario.h"
#include "StageManager.h"
#include "Bowser_Fire.h"

void Bowser::Init(float x, float y)
{
	int handle = LoadGraph("data/image/bowser.png");
	this->position.Set(x, y);
	this->spriteAnimation.InitialAnimation(handle, 4, 3);
	this->spriteAnimation.x = position.x;
	this->spriteAnimation.y = position.y;
	this->RigidBody_collider = Collider(x, y, (float)this->spriteAnimation.sprite.sizeX, (float)this->spriteAnimation.sprite.sizeY);
	this->now_speed_y = 0.0f;
	this->now_speed_x = 1.0f;
	this->direction = -1.0f;
	this->layer = PhysicsLayer::ENEMY;
	this->HP = MAX_HP;
	this->direction = -1.0f;
	this->isLookRight = false;
	this->gravity = 0.1f; //overide gravity
	//this->isTrigger = true;
}


void Bowser::Update(Camera& camera)
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
	{
		//Jump every 2s
		timerJump.Update();

		if (timerJump.HitAndReset())
		{
			Jump();
		}



		//RigidBody update
		PhysicsUpdate();
	}

	//Shoot fire every 3-5s
	timerShoot.Update();
	if (timerShoot.TimerHit())
	{
		timerShoot.SetTimer(GetRandomF(3.0f, timerShoot.MAX_TIMER));
		ShootProjectile();
	}

	//flip direction when out of bounds
	bool isOutOfBound = false;
	if (position.x < (float)(135 * 64) ||
		position.x >(float)(138 * 64))
	{
		isOutOfBound = true;
	}

	if (isOutOfBound)
	{
		direction *= -1.0f;
	}

	now_speed_x = direction;
}

void Bowser::WaitForCamera(Camera& camera)
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

// 横からマリオがぶつかった時の処理
void Bowser::OnHitSide(RigidBody& player)
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

void Bowser::OnHitTop(RigidBody& player)
{
	//
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		mario->ToDeadState();
	}
}

void Bowser::RenderGlobal(Camera& camera, const Mario& mario)
{
	//Check isLookingRight based on mario position
	if (mario.position.x + BLOCK_SIZE / 2.0f < position.x + this->spriteAnimation.sprite.sizeX / 2.0f)
	{
		isLookRight = false;
	}
	else
	{
		isLookRight = true;
	}
	camera.GlobalRenderAnimation(spriteAnimation, isLookRight);
	
	//Debug
	DrawFormatString(200, 200, GetColor(255, 255, 255), "timer: %f", timerShoot.GetCurrentTimer());
}

void Bowser::TakeDamage(RigidBody& attacker)
{
	state = EnemyState::DEAD;
}

void Bowser::Jump()
{
	now_speed_y -= 5.0f;
}

void Bowser::ShootProjectile()
{
	BowserFire* newBowserFire = new BowserFire();
	newBowserFire->Init(position.x, position.y + spriteAnimation.sprite.sizeY / 2.0f);
	StageManager::GetInstance().bowserFire.push_back(newBowserFire);
	RigidBody::collidables.push_back(newBowserFire);
}