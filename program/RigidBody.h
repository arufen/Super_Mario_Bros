#pragma once
#include "HelpfulFunc.h"
#include "Collider.h"
#include "Collidable.h"
#include <vector>
using namespace std;

class RigidBody
{
public:
	static constexpr float GRAVITY = 1.0f;	// gravity (重力)
	Float2 position; //Mario position (座標)
	float now_speed_x;	// 現在の横方向の速度
	float now_speed_y;	// 現在の縦方向の速度
	Collider RigidBody_collider; // Collider (マリオの当たり判定)
	//Check Every collision that has registered here （登録した当たり判定をチェックする）
	static vector<Collidable*> collidables; // all blocks register here to update the collision（登録の当たり判定）

	//Raycast
	Collidable* Raycast(float x, float y); //to Check further collider

	bool hitVertical = false;

	//Resolve collision for a block
	virtual void ResolveCollision(Collidable& block);

	//Main thread
	void PhysicsUpdate(); //Resolve collision for every block


};