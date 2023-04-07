#include <stdlib.h>
#include "GameUtil.h"
#include "ExtraLife.h"
#include "BoundingShape.h"

// Works just like asteroid and other powerups to spawn in a random position
ExtraLife::ExtraLife(void) : GameObject("ExtraLife")
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

ExtraLife::~ExtraLife(void)
{
}
// Checks for the collision of the extra life
bool ExtraLife::CollisionTest(shared_ptr<GameObject> o)
{
	// Extra Life will ignore everything but bullets and the spaceship
	if (o->GetType() == GameObjectType("Bullet") || o->GetType() == GameObjectType("Spaceship")) {
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	else return false;
}
// Handles the collision of the extra life
void ExtraLife::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}