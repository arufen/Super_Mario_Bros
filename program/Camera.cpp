#include "Camera.h"

//Constructor
Camera::Camera() 
{
	pos = { 0.0f, 0.0f };
}

void Camera::SetPos(Float2 targetPos)
{
	pos.Set(targetPos);
}

