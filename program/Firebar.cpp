#include "FireBar.h"
#include "Dxlib.h"
#include <cmath>

#define PI 3.141592653589793

FireBar::FireBar()
{
    pos.Set(0.0f, 0.0f);
    currentAngle = 0.0f;
    rotationSpeed = 2.0f;
    distanceBetweenFire = 24.0f;
    fireCount = 0;
}

void FireBar::Init(Float2 centerPos, int blockHandle, int fireHandle,int dir, int fireballCount)
{
    pos = centerPos;
    fireGraphHandle = fireHandle;
    fireCount = fireballCount;
    rotation_dir = dir;

    image.InitialImageAndSize(blockHandle);
    image.pos = pos;
    collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);

    fireballs.resize(fireCount);

    // FIX: Initialize your fireball sprite strip (2 frames, running at 15 FPS)
    fireAnimation.InitialAnimation(fireGraphHandle, 1, 30);
}

void FireBar::Update()
{
    if(rotation_dir == CLOCKWISE)
    { 
    currentAngle += rotationSpeed;
    }
    else if (rotation_dir == COUNTERCLOCKWISE)
    {
        currentAngle -= rotationSpeed;
    }
    if (currentAngle >= 360.0f) currentAngle -= 360.0f;

    // FIX: Keep the fireball graphic frames cycling forward
    fireAnimation.AnimationUpdateLoop();

    float radians = currentAngle * (PI / 180.0f);

    float pivotX = pos.x + (image.sizeX / 2.0f);
    float pivotY = pos.y + (image.sizeY / 2.0f);

    for (int i = 0; i < fireCount; i++)
    {
        float currentDistance = (i + 1) * distanceBetweenFire;

        fireballs[i].x = pivotX + cosf(radians) * currentDistance;
        fireballs[i].y = pivotY + sinf(radians) * currentDistance;

        // Assign positions directly to the inherited collider object
        float fireSize = 16.0f;
        fireballs[i].collider.x = fireballs[i].x - (fireSize / 2.0f);
        fireballs[i].collider.y = fireballs[i].y - (fireSize / 2.0f);
        fireballs[i].collider.width = fireSize;
        fireballs[i].collider.height = fireSize;
    }
}

// Change this signature line:
void FireBar::Render(Camera camera)
{
    // 1. Render the main solid stone base block
    camera.GlobalRenderImage(image);

    // 2. Render all the spinning fireballs
    for (int i = 0; i < fireCount; i++)
    {
        // Center the 32x32 frame output smoothly on our calculation vector points
        float targetWorldX = fireballs[i].x - ((float)fireAnimation.sprite.sizeX / 2.0f);
        float targetWorldY = fireballs[i].y - ((float)fireAnimation.sprite.sizeY / 2.0f);

        // Convert world spaces to Camera viewport space!
        float screenX = targetWorldX - camera.pos.x;
        float screenY = targetWorldY - camera.pos.y;

        // Render directly onto screen space
        fireAnimation.AnimationRender(screenX, screenY);
    }
}