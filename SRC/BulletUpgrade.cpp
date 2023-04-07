#include <stdlib.h>
#include "GameUtil.h"
#include "BulletUpgrade.h"
#include "BoundingShape.h"

// Bullet upgrade works just like asteroid and other powerups to spawn in a random position
BulletUpgrade::BulletUpgrade(void) : GameObject("BulletUpgrade")
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

BulletUpgrade::~BulletUpgrade(void)
{
}
// Checks for the collision of the bullet upgrade
bool BulletUpgrade::CollisionTest(shared_ptr<GameObject> o)
{
	// Bullet upgrade will ignore everything but bullets and the spaceship
	if (o->GetType() == GameObjectType("Bullet") || o->GetType() == GameObjectType("Spaceship")) {
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	else return false;
}
// Handles the collision of the bullet upgrade
void BulletUpgrade::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}