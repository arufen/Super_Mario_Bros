#pragma once

#include "Camera.h"
#include "Hit.h"
#include "HelpfulFunc.h"
#include "RigidBody.h"

enum class EnemyState
{
	WAITING, //Wait until enemy is inside camera
	ACTIVE,
	DEAD,
};

class Enemy
{
public:


	EnemyState state = EnemyState::WAITING;


	//Update
	virtual void WaitForCamera(Camera& camera) = 0; //Change enemy state to ACTIVE when inside camera
	virtual void TakeDamage(RigidBody& attacker) = 0;
};