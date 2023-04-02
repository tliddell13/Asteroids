#include <stdlib.h>
#include "GameUtil.h"
#include "ShieldPower.h"
#include "BoundingShape.h"

// Change random position to not spawn where the player is at
ShieldPower::ShieldPower(void) : GameObject("ShieldPower")
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

ShieldPower::~ShieldPower(void)
{
}
// Checks for the collision of an asteroid
bool ShieldPower::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	// Bullet upgrade will ignore everything but bullets and the spaceship
	if (o->GetType() == GameObjectType("Bullet") || o->GetType() == GameObjectType("Spaceship")) {
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	else return false;
}
// Handles the collision of an asteroid
void ShieldPower::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}