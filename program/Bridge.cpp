//#include "Bridge.h"
//
//void Bridge::Init(float x, float y)
//{
//	int handle = LoadGraph("data/image/bridge.png");
//	this->position.Set(x, y);
//	this->image.pos = position;
//	this->image.InitialImageAndSize(handle);
//	this->collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);
//
//}
//
//void Bridge::RenderGlobal(Camera& camera)
//{
//	camera.GlobalRenderImage(image);
//}