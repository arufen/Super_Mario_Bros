#pragma once

#include "Camera.h"

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


	//void WaitForCamera(Camera& camera);
};