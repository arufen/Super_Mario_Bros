#include "Axe.h"
#include "Mario.h"
#include "StageManager.h"

void Axe::Init(float x, float y)
{
	int handle = LoadGraph("data/image/axe.png");
	this->position.Set(x, y);
	this->spriteAnimation.InitialAnimation(handle, 3, 6);
	this->spriteAnimation.x = position.x;
	this->spriteAnimation.y = position.y;
	this->collider = Collider(x, y, (float)this->spriteAnimation.sprite.sizeX, (float)this->spriteAnimation.sprite.sizeY);
	this->isActive = true;
	this->isTrigger = true;
}

void Axe::Update()
{

	//image update
	spriteAnimation.x = position.x;
	spriteAnimation.y = position.y;
	spriteAnimation.AnimationUpdateLoop();

}

void Axe::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderAnimation(spriteAnimation);
}


void Axe::Deactivate()
{
	isActive = false;
}

void Axe::OnHitSide(RigidBody& player)
{
	// すでに踏まれて死んでいる（潰れかけの）クリボーなら判定しない
	if (!isActive) return;

	// 当たってきたオブジェクトがマリオかどうかをチェック
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		ClearBridge();
	}
}

void Axe::ClearBridge()
{
	StageManager::GetInstance().isBridgeClearing = true;

	Deactivate();
}
