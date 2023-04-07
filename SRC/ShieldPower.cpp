#include <stdlib.h>
#include "GameUtil.h"
#include "ShieldPower.h"
#include "BoundingShape.h"

// The class for the shield object floating around
ShieldPower::ShieldPower(void) : GameObject("ShieldPower")
{
	// Spawns in a random position just like an asteroid and other powerups
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
// Checks for the collision of the shield
bool ShieldPower::CollisionTest(shared_ptr<GameObject> o)
{
	// Bullet upgrade will ignore everything but bullets and the spaceship
	if (o->GetType() == GameObjectType("Bullet") || o->GetType() == GameObjectType("Spaceship")) {
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	else return false;
}
// Handles the collision of the shield
void ShieldPower::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}