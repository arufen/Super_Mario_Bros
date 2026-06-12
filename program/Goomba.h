#pragma once

#include "Enemy.h"

// Goomba は Enemy を継承するように変更
class Goomba : public Enemy
{
public:
    Image image;
    float direction;

    // 基底クラスの仮想関数をオーバーライド
    void Init(float x, float y, int handle) override;
    void Update() override;
    void RenderGlobal(Camera& camera) override;

    // マリオと衝突したときのイベント関数
    void OnHitTop(RigidBody& player) override;    // 上から踏まれた
    void OnHitSide(RigidBody& player) override;   // 横からぶつかった
    void OnHitBottom(RigidBody& player) override; // 下からぶつかった
};