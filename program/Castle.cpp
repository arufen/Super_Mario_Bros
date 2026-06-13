#include "Castle.h"
#include "DxLib.h"

Castle::Castle() {}

void Castle::Init(float x, float y)
{
	image.InitialImageAndSize(LoadGraph("data/image/castle_1-1.png"));
	image.pos.Set(x, y);
}

void Castle::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderImage(image);
}