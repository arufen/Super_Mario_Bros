#pragma once
#include "Float2.h"

class Timer
{
public:

	//Max Timer
	float MAX_TIMER;


	//Functions
	void Update();
	bool TimerHit();
	void ResetTimer();
	void SetTimer(float seconds);
	float GetCurrentTimer();

	//return true and reset
	bool HitAndReset();

	Timer(float maxTimer) : MAX_TIMER(0), currentFrame(0)
	{
		this->MAX_TIMER = maxTimer;
		currentFrame = static_cast<int>(MAX_TIMER * 60); //Convert timer to frame
	}

private:
	//current frame
	float currentFrame;
};

//better version for image
class Image
{
public:
	int image;
	Float2 pos;
	int sizeX, sizeY;

	//Method
	void InitialImageAndSize(int Loadimage);
	void RenderCenter();
	void RenderCenter(Float2 targetPos);

	Image() : image(-1), pos(0.0f, 0.0f), sizeX(0), sizeY(0) {}
};

enum class AnimationState
{
	ENTERING,
	PLAYING,
	COMPLETED,
};

class Animation
{
public:
	float x, y;
	Image sprite;
	int frameCount; //total frames
	int currentFrame; //current frame
	int animationSpeed; //frames per second
	int FPS;
	AnimationState state;
	Animation() : x(0), y(0), sprite(), frameCount(0), currentFrame(0), animationSpeed(0), FPS(0), state(AnimationState::COMPLETED) {}

	//Functions
	void InitialAnimation(int image, int frameCount, int FPS);
	void AnimationUpdateLoop();
	void AnimationUpdateOnce();
	void PlayAnimation(); //only used for AnimationUpdateOnce
	void AnimationRender(const float x, const float y);
	void AnimationRenderCenter(bool reverseFlag = false);
};


float Lerp(float a, float b, float t); //smooth interpolation
float GetAngle(float x1, float y1, float x2, float y2); //get angle point to point
//NOTE: THE ANGLE RETURNS RANGING FROM -180 TO 180
//float GetDistance(float x1, float y1, float x2, float y2);
float ToRadius(float angle);
void MoveTowards(float& x, float& y, float rot, float speed);
bool HitDetectBox(float x1, float y1, float x2, float y2, Image sprite1, Image sprite2);
void easeOutBounce(
	float& x,
	float fromX,
	float toX,
	float& frameTime,
	float durationSeconds,
	float fps = 60.0f
);