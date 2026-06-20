#include "Moving_Platform.h"
#include "Mario.h"

void MovingPlatform::Init(float ax, float ay, float bx, float by)
{
	int handle = LoadGraph("data/image/platform.png");
	this->pointA.Set(ax, ay);
	this->pointB.Set(bx, by);
	this->position = pointA; // start at point A
	this->image.InitialImageAndSize(handle);
	this->collider = Collider(ax, ay, (float)this->image.sizeX, (float)this->image.sizeY);
	this->now_speed_x = 0.0f;
	this->direction = 1.0f; // 1 = heading toward B, -1 = heading back toward A
	this->layer = PhysicsLayer::ENEMY;
	this->isActive = true;
	this->isTrigger = true; //false when mario above it
}



void MovingPlatform::Update(const Mario& mario)
{
	float prevX = position.x; // remember where we were before this frame's move

	// move t back and forth between 0 and 1, this is what makes it bounce A <-> B
	t += lerpSpeed * direction;
	if (t >= 1.0f) { t = 1.0f; direction = -1.0f; }
	if (t <= 0.0f) { t = 0.0f; direction = 1.0f; }

	// smoothly interpolate position between point A and point B
	position.x = Lerp(pointA.x, pointB.x, t);
	position.y = Lerp(pointA.y, pointB.y, t);

	now_speed_x = position.x - prevX; // how far we ACTUALLY moved this frame, used to carry Mario along

	//Collider
	collider.x = position.x;
	collider.y = position.y;

	//image update
	image.pos = position;

	float offsetCollider = 10.0f; //so it doesn't check under 1 pixel

	//turn on collider when mario above the platform
	if (mario.position.y + mario.RigidBody_collider.height < position.y + offsetCollider)
	{
		isTrigger = false;
	}
	else
	{
		isTrigger = true;
	}
}


void MovingPlatform::OnHitTop(RigidBody& player)
{
	Mario* mario = dynamic_cast<Mario*>(&player);
	if (mario != nullptr)
	{
		mario->position.x += now_speed_x; // carry mario along by however far the platform moved this frame
	}
}


void MovingPlatform::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderImage(image);
}

void MovingPlatform::Deactivate()
{
	isActive = false;
}