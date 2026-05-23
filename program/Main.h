#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <time.h>
#include <math.h>

#pragma warning (disable : 4819)

#include <d3d9.h>
//#include <d3dx9.h>

#include <DxLib.h>

// キーボードやマウスの入力を取得するためのヘルパー関数群
bool PushHitKey( int key );
bool CheckMouseInput( int button = MOUSE_INPUT_LEFT|MOUSE_INPUT_RIGHT );
bool PushMouseInput( int button = MOUSE_INPUT_LEFT|MOUSE_INPUT_RIGHT );
int GetMouseX();
int GetMouseY();

// 角度計算用のユーティリティ関数群
float TO_RADIAN( float degree );
float TO_DEGREE( float radian );
float GetLength_RotSotte( float rot );
float GetLength_RotMukai( float rot );
float GetRadian_LengthMukaiSotte( float mukai, float sotte );
