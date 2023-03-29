#include <stdlib.h>
#include "GameUtil.h"
#include "PointBonus.h"
#include "BoundingShape.h"

// Change random position to not spawn where the player is at
PointBonus::PointBonus(void) : GameObject("PointBonus")
{
	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD * mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD * mAngle);
	mVelocity.z = 0.0;
}

PointBonus::~PointBonus(void)
{
}
// Checks for the collision of the bonus
bool PointBonus::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	// Point bonus will ignore the asteroids
	if (o-> GetType() == GameObjectType("Asteroid")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
// Handles the collision of the bonus
void PointBonus::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}