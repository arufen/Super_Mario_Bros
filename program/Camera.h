#pragma once
#include "Float2.h"

class Camera
{
public: 
	Float2 pos;

	//Constructor
	Camera();

	void SetPos(Float2 targetPos);
};