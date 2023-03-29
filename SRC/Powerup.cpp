#include <stdlib.h>
#include "GameUtil.h"
#include "Powerup.h"
#include "BoundingShape.h"

/*
* This is the class for the powerups as they are floating around the game world before the collide. 
* The possible powerups that are randomly spawned are:
* - Extra life
* - Bonus points
* - Shield 
* - Bullet upgrade
* - Seeking Rocket
* When the powerup is collided with, one of the following happens
* - The bonus is applied (Extra life, Bonus points)
* - The powerup is created and follows player(shield)
* - The user is equipped with powerup(rocket an bullet upgrade)
*/

Powerup::Powerup(char const * const powerupType) : GameObject(powerupType)
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

Powerup::~Powerup(void)
{
}
// Checks for the collision of a powerup
bool Powerup::CollisionTest(shared_ptr<GameObject> o)
{
	// Switch statement based on powerup
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
// Handles the collision of a powerup
void Powerup::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}