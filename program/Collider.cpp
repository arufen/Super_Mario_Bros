#include "Collider.h"

bool Collider::intersects(const Collider& other) const {

	return CheckBoxHit(x, y, width, height, other.x, other.y, other.width, other.height);
}