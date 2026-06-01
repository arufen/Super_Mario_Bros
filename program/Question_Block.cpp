#include "Question_Block.h"
#include "DxLib.h"

QuestionBlock::QuestionBlock()
{
	active = false;
	isBouncing = false;
	isAvailable = true;
	bounceTimer = 0.0f;
}

void QuestionBlock::Init(Float2 startPos, int active_graph, int empty_graph)
{
	pos = startPos;
	originalPos = startPos;
	texActive = active_graph;
	texEmpty = empty_graph;
	active = true;

	image.InitialImageAndSize(texActive);
}
void QuestionBlock::Bump()
{
	if (!isAvailable || isBouncing) return;

	isBouncing = true;
	bounceTimer = 0.0f;
}
void QuestionBlock::Update()
{
	if (!active)return;

	if (isBouncing)
	{
		bounceTimer += 1.0f;

		// Fast manual arcade bounce (Up for 6 frames, down for 6 frames)
		if (bounceTimer <= 6.0f)
		{
			pos.y -= 3.0f;
		}
		else if (bounceTimer <= 12.0f)
		{
			pos.y += 3.0f;
		}
		else
		{
			pos.y = originalPos.y; // Pin it back to original positioning exactly
			isBouncing = false;
			isAvailable = false;

			image.InitialImageAndSize(texEmpty);
		}
	}
}
void QuestionBlock::Render(Float2 cameraPos)
{
	if (!active) return;

	Float2 screenPos = { pos.x - cameraPos.x,pos.y - cameraPos.y };
	image.Render(screenPos);
}