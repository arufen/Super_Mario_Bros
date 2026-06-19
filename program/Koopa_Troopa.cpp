#include "Koopa_Troopa.h"
#include "Sound.h"

void KoopaTroopa::Init(float x, float y)
{
	int handle = LoadGraph("data/image/KoopaTroopa.png");
	int handleShell = LoadGraph("data/image/KoopaTroopaShell.png");
	this->position.Set(x, y);

	//not in shell sprite
	this->spriteAnimation.InitialAnimation(handle, 2, 6);
	this->spriteAnimation.x = position.x;
	this->spriteAnimation.y = position.y;

	//shell 
	this->spriteAnimationShell.InitialAnimation(handleShell, 2, 6);
	this->spriteAnimationShell.x = position.x;
	this->spriteAnimationShell.y = position.y;

	this->RigidBody_collider = Collider(x, y, (float)this->spriteAnimation.sprite.sizeX, (float)this->spriteAnimation.sprite.sizeY);
	this->now_speed_y = 0.0f;
	this->now_speed_x = 1.0f;
	this->direction = -1.0f;
	this->layer = PhysicsLayer::ENEMY;
	this->isInShell = false;
	this->isShellMoving = false;
}


void KoopaTroopa::Update(Camera& camera, Mario& mario)
{
	

	if (isInShell && isShellMoving)
	{
		now_speed_x = direction * SHELL_SPEED;
	}
	else if (!isInShell)
	{
		now_speed_x = direction * DEFAULT_SPEED;
	}

	//Collider
	RigidBody_collider.x = position.x;
	RigidBody_collider.y = position.y;

	collider = RigidBody_collider;

	//image update
	if (!isInShell)
	{
		spriteAnimation.x = position.x;
		spriteAnimation.y = position.y;
		spriteAnimation.AnimationUpdateLoop();
	}
	else
	{
		spriteAnimationShell.x = position.x;
		spriteAnimationShell.y = position.y;
	}

	if (isShellMoving)
	{
		spriteAnimationShell.AnimationUpdateLoop();
	}
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

}

void KoopaTroopa::WaitForCamera(Camera& camera)
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

void KoopaTroopa::OnHitTop(RigidBody& player)
{
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario == nullptr) return;

	if (mario->starEffect.isActive) ToDeadState();
	
	mario->Jump(false);
	TakeDamage(player);
	
}

void KoopaTroopa::OnHitSide(RigidBody& player)
{
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario == nullptr) return;

	if (mario->starEffect.isActive) ToDeadState();

	if (isShellMoving)
	{
		mario->ToDeadState();
	}
	else if (isInShell && !isShellMoving)
	{
		TakeDamage(player); // kicks the shell, doesn't kill mario
	}
	else // normal koopa, not in shell
	{
		mario->ToDeadState();
	}


	
}

void KoopaTroopa::RenderGlobal(Camera& camera)
{
	bool lookRight = direction < 0.0f ? false : true;
	if (!isInShell)
	{
		camera.GlobalRenderAnimation(spriteAnimation, lookRight);
	}
	else
	{
		camera.GlobalRenderAnimation(spriteAnimationShell);
	}
}

void KoopaTroopa::TakeDamage(RigidBody& attacker)
{
	//only react if it's mario hitting it
	Mario* mario = dynamic_cast<Mario*>(&attacker);
	if (mario != nullptr)
	{
		//first hit
		if (!isInShell && !isShellMoving)
		{
			isInShell = true;
			now_speed_x = 0.0f; // stop moving when it goes into shell

			//change hitbox to shell sprite
			RigidBody_collider = Collider(position.x, position.y, (float)this->spriteAnimationShell.sprite.sizeX, (float)this->spriteAnimationShell.sprite.sizeY);

			// ì•Ç‹ÇÍÇΩéûÇÃSEÇçƒê∂
			SoundManager::GetInstance().PlaySE("Stomp");

			return;
		}
		else if (isInShell && !isShellMoving)
		{
			// check which side mario is on
			if (mario->position.x < position.x)
				direction = 1.0f; // shell goes right
			else
				direction = -1.0f; // shell goes left


			//after kicking shell gives invincible
			mario->invincibleTimer.SetTimer(0.2f);
			isShellMoving = true;

			// èRÇ¡ÇΩéûÇÃSEÇçƒê∂
			SoundManager::GetInstance().PlaySE("Kick");
		}


	}

}

void KoopaTroopa::ToDeadState()
{
	state = EnemyState::DEAD;
}