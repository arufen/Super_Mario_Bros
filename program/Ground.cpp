#include "Ground.h"
#include "Main.h"
void Ground::Init(float x, float y, int handle)
{
	this->position.Set(x, y);
	this->image.pos = position;
	this->image.InitialImageAndSize(handle);
	this->collider = Collider(x, y, (float)this->image.sizeX, (float)this->image.sizeY);

}

void Ground::Update(Mario& mario)
{
    //// skip if not overlapping (当たり判定してないとき）
    //if (!collider.intersects(mario.collider)) return;

    //// push player up (stand on top)　（マリオが上にいると座標を修正する）
    //float overlapY = collider.y - (mario.collider.y + mario.collider.height);
    //if (collider.intersects(mario.collider))
    //{
    //    mario.position.y += overlapY;
    //    mario.collider.y += overlapY;
    //    mario.now_speed_y = 0;
    //    mario.isJumping = false;
    //    mario.jumpHoldTimer = 0.0f;
    //}

    if (!collider.intersects(mario.collider)) return;

    float overlapLeft = (mario.collider.x + mario.collider.width) - collider.x;
    float overlapRight = (collider.x + collider.width) - mario.collider.x;
    float overlapTop = (mario.collider.y + mario.collider.height) - collider.y;
    float overlapBottom = (collider.y + collider.height) - mario.collider.y;

    float minX = min(overlapLeft, overlapRight);
    float minY = min(overlapTop, overlapBottom);

    // bias fix — only treat as side hit if X overlap is clearly smaller
    const float bias = 20.0f; // tweak this value if needed

    if (minY < minX + bias)
    {
        // vertical hit
        if (overlapTop < overlapBottom)
        {
            mario.position.y -= overlapTop;
            mario.collider.y -= overlapTop;
            mario.now_speed_y = 0;
            mario.isJumping = false;
            mario.jumpHoldTimer = 0.0f;
        }
        else
        {
            mario.position.y += overlapBottom;
            mario.collider.y += overlapBottom;
            mario.now_speed_y = 0;
        }
    }
    else
    {
        // horizontal hit
        if (overlapLeft < overlapRight)
        {
            mario.position.x -= overlapLeft;
            mario.collider.x -= overlapLeft;
        }
        else
        {
            mario.position.x += overlapRight;
            mario.collider.x += overlapRight;
        }
        mario.now_speed_x = 0;
    }
 
}

void Ground::RenderGlobal(Camera& camera)
{
	camera.GlobalRenderImage(image);


    //debug
	/*camera.GlobalRenderBox(collider.x, collider.y, (collider.x + collider.width), (collider.y + collider.height), GetColor(255, 0, 0), false);*/
	/*DrawCircleAA(position.x, position.y, 5, 10, GetColor(0, 255, 0), true);*/

	
    
}