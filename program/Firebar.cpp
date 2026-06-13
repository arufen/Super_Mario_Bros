#include "FireBar.h"
#include "Dxlib.h"
#include <cmath>

#define PI 3.141592653589793

FireBar::FireBar()
{
    pos.Set(0.0f, 0.0f);
    currentAngle = 0.0f;
    rotationSpeed = 2.0f;
    distanceBetweenFire = 28.0f;
    fireCount = 0;
}

void FireBar::Init(Float2 centerPos, int blockHandle, int fireHandle, RotationDir dir, int fireballCount, float startAngle)
{
    pos = centerPos;
    fireGraphHandle = fireHandle;
    fireCount = fireballCount;
    rotation_dir = dir;

    currentAngle = startAngle;

    image.InitialImageAndSize(blockHandle);
    image.pos = pos;
    collider = Collider(pos.x, pos.y, (float)image.sizeX, (float)image.sizeY);

    // Resizing the vector triggers the Fireball() constructor, enabling isTrigger = true automatically
    fireballs.resize(fireCount);

    fireAnimation.InitialAnimation(fireGraphHandle, 4, 15);
}

void FireBar::Update()
{
    fireAnimation.AnimationUpdateLoop();

    if (rotation_dir == RotationDir::CLOCKWISE)
    {
        currentAngle += rotationSpeed;
    }
    else if (rotation_dir == RotationDir::COUNTERCLOCKWISE)
    {
        currentAngle -= rotationSpeed;
    }
    if (currentAngle >= 360.0f) currentAngle -= 360.0f;

    float pivotX = pos.x + (image.sizeX / 2.0f);
    float pivotY = pos.y + (image.sizeY / 2.0f);

    float radians = currentAngle * (PI / 180.0f);

    for (int i = 0; i < fireCount; i++)
    {
        float currentDistance = i * distanceBetweenFire;
        float fireSize = 16.0f;

        // Calculate absolute world positioning coordinates
        float targetX = pivotX + cosf(radians) * currentDistance;
        float targetY = pivotY + sinf(radians) * currentDistance;

        // --- UPDATE DIRECTLY TO INHERITED COLLIDER ---
        fireballs[i].collider.width = fireSize;
        fireballs[i].collider.height = fireSize;
        fireballs[i].collider.x = targetX - (fireSize / 2.0f);
        fireballs[i].collider.y = targetY - (fireSize / 2.0f);
    }
}

void FireBar::Render(Camera camera)
{
    // 1. Render the main center block anchor
    camera.GlobalRenderImage(image);

    // 2. Render all straight-line fireballs with staggered look angles
    for (int i = 0; i < fireCount; i++)
    {
        // Compute center point using the formal bounding box collider properties
        float centerX = fireballs[i].collider.x + (fireballs[i].collider.width / 2.0f);
        float centerY = fireballs[i].collider.y + (fireballs[i].collider.height / 2.0f);

        // Position the target graphic asset cleanly over the physics vector points
        fireAnimation.x = centerX - ((float)fireAnimation.sprite.sizeX / 2.0f);
        fireAnimation.y = centerY - ((float)fireAnimation.sprite.sizeY / 2.0f);

        int backupMasterFrame = fireAnimation.currentFrame;

        // Shift visual layout look frames cleanly down the line
        fireAnimation.currentFrame = (backupMasterFrame + i) % fireAnimation.frameCount;

        camera.GlobalRenderAnimation(fireAnimation);

        fireAnimation.currentFrame = backupMasterFrame;
    }
}