#pragma once
#include "Hit.h"

//Create box collider
class Collider {
public:
    float x, y, width, height;

	//Default constructor 
    Collider() : x(0), y(0), width(0), height(0) {}
    //Alternate constructor
    Collider(float x, float y, float w, float h)
        : x(x), y(y), width(w), height(h) {
    }

	//check if this collider intersects with another collider (“–‚½‚è”»’è‚ª‚ ‚Á‚½‚çj
    bool intersects(const Collider& other) const;

  
};