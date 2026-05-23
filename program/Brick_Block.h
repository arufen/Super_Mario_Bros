#pragma once
#include "HelpfulFunc.h"

class BrickBlock
{
public:
	void Init();
	void Set(float x, float y);
	void Update();
	void Render();

private:
	Float2 pos;
};