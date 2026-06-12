#pragma once
#include "RigidBody.h"
#include "Collidable.h"
#include "Camera.h"

// すべてのエネミーの基底クラス
class Enemy : public RigidBody, public Collidable
{
public:
    virtual ~Enemy() = default;

    // 派生クラス（クリボーなど）で必ず固有の実装をさせる仮想関数
    virtual void Init(float x, float y, int handle) = 0;
    virtual void Update() = 0;
    virtual void RenderGlobal(Camera& camera) = 0;
};