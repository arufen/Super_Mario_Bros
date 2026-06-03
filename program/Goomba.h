#include "RigidBody.h"
#include "HelpfulFunc.h"

//Todo:
//Render Add Image,



class Goomba : public RigidBody
{
	Image image;

	//Main thread
	void Init();
	void Update();
	void Render();
};