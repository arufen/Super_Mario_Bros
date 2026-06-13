#pragma once
#include "Camera.h"

class Castle
{
public:
	Image image;

	Castle();

	// x, y はワールド座標(ピクセル)
	void Init(float x, float y);

	// アニメーションがないならUpdateは不要ですが、描画用の関数を用意します
	void RenderGlobal(Camera& camera);
};