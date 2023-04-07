#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"
#include "Spaceship.h"

Asteroid::Asteroid(shared_ptr<Spaceship> spaceship) : GameObject("Asteroid"), mSpaceship(spaceship)
{
	/* 
	* Use the spaceship as a parameter for the position 
	* to avoid spawnning an asteroid on top of the player.
	*/ 
	// The spaceship will also be used to make the asteroids not be destroyed 
	// when the spaceship is invincible
	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	double x = rand() / 2;
	// Keep picking until x does not equal within 10 units  of spaceship's x
	while (abs(x - spaceship->GetPosition().x) < 10) {
		x = rand() / static_cast<double>(2);
	}
	double y = rand() / 2;
	// keep picking until y does not equal within 10 units of spaceship's y
	while (abs(y - spaceship->GetPosition().y) < 10) {
		y = rand() / static_cast<double>(2);
	}
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD*mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD*mAngle);
	mVelocity.z = 0.0;
}

Asteroid::~Asteroid(void)
{
}
// Checks for the collision of an asteroid
bool Asteroid::CollisionTest(shared_ptr<GameObject> o)
{	// Asteroids will ignore everything but bullets and the spaceship
	// Asteroids are not destroyed when the player is still invincible
	if (mSpaceship->IsInvincible()) return false;
	if (o->GetType() == GameObjectType("Bullet") || o->GetType() == GameObjectType("Spaceship") || o->GetType() == GameObjectType("Shield")) {
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	return false;
}
// Handles the collision of an asteroid
void Asteroid::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}

