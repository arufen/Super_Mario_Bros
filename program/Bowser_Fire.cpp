#include "Bowser_Fire.h"
#include "StageManager.h"
#include "Sound.h"
#include "Main.h"

extern Camera MainCamera;

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
	this->hasPlayedSE = false;
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

	if (!hasPlayedSE && isActive)
	{
		Float2 cameraSize{ SCREEN_W, SCREEN_H };
		Float2 fireSize{ (float)spriteAnimation.sprite.sizeX, (float)spriteAnimation.sprite.sizeY };

		// メインカメラの範囲内と、炎の当たり判定が重なった時（＝画面に映った時）
		if (CheckBoxHit(MainCamera.pos, cameraSize, position, fireSize))
		{
			SoundManager::GetInstance().PlaySE("BowserFire"); // 音を鳴らす
			hasPlayedSE = true; // 鳴らしたフラグを立てて、2回目以降鳴らないようにする
		}
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
