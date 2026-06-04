#include "RigidBody.h"
#include "HelpfulFunc.h"
#include "Camera.h"

//Todo:
//Render Add Image,



class Goomba : public RigidBody
{
public:
	Image image;

	//Main thread
	void Init(float x, float y, int handle);
	void Update();
	void GlobalRender(Camera& camera);
};