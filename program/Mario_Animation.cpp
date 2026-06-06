#include "Mario_Animation.h"

void MarioAnimation::Init()
{
	walkAnimation.InitialAnimation(LoadGraph("data/mario/mario_walk_01.png"), 3, 15);
	jumpAnimation.InitialAnimation(LoadGraph("data/mario/mario_jump_01.png"), 3, 15);
}
void MarioAnimation::Update()
{
	// アニメーションの更新は、マリオの状態や速度
	
}
void MarioAnimation::Render()
{

	walkAnimation.AnimationRender(walkAnimation.x, walkAnimation.y);

	// アニメーションの描画は、マリオの現在の状態
	
}