#include "Main.h"
#include "Game.h"
#include "Float2.h"
#include "Hit.h"
#include "Random.h"
#include "Camera.h"
#include "HelpfulFunc.h"

Camera MainCamera;

//Map
Image mapImage; 

//Render in global (calculated based on camera)
void GlobalRenderImage(Camera camera, Image image)
{
	Float2 globalPos;
	globalPos.Set(image.pos.x - camera.pos.x, image.pos.y - camera.pos.y);
	image.RenderCenter(globalPos);
}

//---------------------------------------------------------------------------------
//	èâä˙âªèàóù
//---------------------------------------------------------------------------------
void GameInit()
{
	//map init
	mapImage.InitialImageAndSize(LoadGraph("data/1-1.png")); 
	mapImage.pos.Set(0.0f, SCREEN_H / 2);
	MainCamera.pos.Set(100.0f, 0.0f);
}
//---------------------------------------------------------------------------------
//	çXêVèàóù
//---------------------------------------------------------------------------------
void GameUpdate()
{
	float  cameraSpeed = 2.0f;
	if(CheckHitKey(KEY_INPUT_RIGHT))
	{
		MainCamera.pos.x += cameraSpeed;
	}
	if (CheckHitKey(KEY_INPUT_LEFT))
	{
		MainCamera.pos.x -= cameraSpeed;
	}
	if (CheckHitKey(KEY_INPUT_UP))
	{
		MainCamera.pos.y -= cameraSpeed;
	}
	if (CheckHitKey(KEY_INPUT_DOWN))
	{
		MainCamera.pos.y += cameraSpeed;
	}
}
//---------------------------------------------------------------------------------
//	ï`âÊèàóù
//---------------------------------------------------------------------------------
void GameRender()
{
	GlobalRenderImage(MainCamera, mapImage);
	DrawFormatString(0, 0, GetColor(255, 255, 255), "camera pos X : %f, camera pos Y : %f", MainCamera.pos.x, MainCamera.pos.y);
}
//---------------------------------------------------------------------------------
//	èIóπèàóù
//---------------------------------------------------------------------------------
void GameExit()
{

}
