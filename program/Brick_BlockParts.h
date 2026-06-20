#pragma once
#include "IBlock.h"
#include "Camera.h"
#include "HelpfulFunc.h"

class BrickBlockParts : public IBlock
{
public:
    BrickBlockParts() :pos{ 0.0f, 0.0F }, velocity{ 0.0f, 0.0f }, angle(0.0f), rotation_speed(0.0f), texHandle(-1), active(false) {}

    void Spawn(Float2 spawnPos, Float2 Velocity, float rot_speed, int graphHandle)
    {
        pos = spawnPos;
        velocity = Velocity;
        rotation_speed = rot_speed;
        texHandle = graphHandle;
        angle = 0.0f;
        active = true;
    }

    void Update()
    {
        if (!active) return;

        const float Gravity = 0.4f;
        velocity.y += Gravity;

        pos.x += velocity.x;
        pos.y += velocity.y;

        angle += rotation_speed;
        if (pos.y >= 15 * BLOCK_SIZE)
        {
            active = false;
        }
    }
    void Render(Camera camera)
    {
        if (!active || texHandle == -1)return;

        int screenX = static_cast<int>(pos.x - camera.pos.x);
        int screenY = static_cast<int>(pos.y - camera.pos.y);

        DrawRotaGraph2(screenX, screenY, 16, 16,1.0f, angle, texHandle, TRUE);
    }
    bool IsActive() const { return active; }
    Float2 GetPos() const override { return pos; }
    Image GetSprite() const override { return image; }

private:
    Float2 pos;          // Dynamic tracking position (bounces during hit sequences)
    Float2 velocity;
    float angle;
    float rotation_speed;
    int texHandle;

    bool active;
    Image image;

};

