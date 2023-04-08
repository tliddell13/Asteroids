#include <stdlib.h>
#include <cmath>
#include "GameUtil.h"
#include "AlienSpaceship.h"
#include "BoundingShape.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "AlienBullet.h"
#include "BoundingSphere.h"

AlienSpaceship::AlienSpaceship(shared_ptr<Spaceship> spaceship) : GameObject("AlienSpaceship"), mSpaceship(spaceship), mThrust(0)
{
	mEvade = false;
	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	double x = 0;
	//If the player is at the spawn position Keep picking until x does not equal within 10 units  of spaceship's x
	while (abs(x - spaceship->GetPosition().x) < 10) {
		x = rand() / 2;
	}
	mPosition.x = x;
	mPosition.y = 100;
	mPosition.z = 0.0;
	mAngle = 180;
	
}

AlienSpaceship::~AlienSpaceship(void)
{
}
// This function is called on a timer to move the alien spaceship
void AlienSpaceship::Thrust(float t)
{
	mThrust = t;
	// Find the distance between the two spaceships
	GLVector3f distanceVector = mSpaceship->GetPosition() - mPosition;
	float distance = distanceVector.length();
	
	GLVector3f direction = mSpaceship->GetPosition() - mPosition;
	direction.normalize();

	// If the distance is greater than 80 the alien spaceship instead tries to cut off the player
	if(distance > 80) {
		direction = mSpaceship->GetPosition() +( mSpaceship->GetVelocity() * 4 )- mPosition;
		direction.normalize();
	}
	// But if the spaceship is invasion mode it does that instead
	
	if (mEvade) {
		if (distance < 70) {
			direction = mSpaceship->GetPosition() - mPosition;
			direction.normalize();
  			mAcceleration.x = direction.x * -mThrust;
			mAcceleration.y = direction.y * -mThrust;
		}
		else mEvade = false;
	}
	
	else if (direction < 0) {
		// Make sure the spaceship isn't going to fast, as it can easily lose control
		// if it is going too fast, start going the opposite direction of the velocity
		// when the speed gets to high
		if (mVelocity.x > -10) {
			mAcceleration.x = -direction.x * mThrust;
		}
		else {
			mAcceleration.x = mThrust;
		}
		if (mVelocity.y > -10) {
				mAcceleration.y = -direction.y * mThrust;
		}
		else {
				mAcceleration.y = mThrust;
		}
	
	}
	else {
		if (mVelocity.x < 10) {
			mAcceleration.x = direction.x * mThrust;
		}
		else {
			mAcceleration.x = -mThrust;
		}
		if (mVelocity.y < 10) {
			mAcceleration.y = direction.y * mThrust;
		}
		else {
			mAcceleration.y = -mThrust;
		}
	}
}
/* Returns the thrust so the timer in Asteroids can know whether
* the alien spaceship is applying thrust */
float AlienSpaceship::GetThrust()
{
	return mThrust;
}

/** Shoot a bullet. */
void AlienSpaceship::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;
	// Find what direction the player is in respect to the alien's position
	GLVector3f shotDirection = mSpaceship->GetPosition() - mPosition;
	shotDirection.normalize();
	// Calculate the point at the node of the spaceship from position and heading
	GLVector3f bullet_position = mPosition + (shotDirection * 4);
	// Calculate how fast the bullet should travel
	// Can't be as fast as the players speed or the alien is too good
	float bullet_speed = 20;
	// Construct a vector for the bullet's velocity
	GLVector3f bullet_velocity = mVelocity + shotDirection * bullet_speed;
	
	if(bullet_velocity.length() < 0) {
		GLVector3f bullet_velocity = -mVelocity + shotDirection * bullet_speed;
	}
	// Construct a new bullet
	shared_ptr<GameObject> bullet
	(new AlienBullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mBulletShape);
	// Add the new bullet to the game world
	mWorld->AddObject(bullet);

}

// Checks for collisions
bool AlienSpaceship::CollisionTest(shared_ptr<GameObject> o)
{
	// Alien Spaceship can be destroyed by a bullet, a spaceship, or a shield
	if (o->GetType() == GameObjectType("Spaceship") || o->GetType() == GameObjectType("Shield")) {
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	if (o->GetType() == GameObjectType("Bullet")) {
			GLVector3f distanceVector = o->GetPosition() - mPosition;
			float distance = distanceVector.length();
			float collisionRadius = 80.0f;
			if (distance < collisionRadius) {
 				mEvade = true;
			}
			return mBoundingShape->CollisionTest(o->GetBoundingShape());
		}
	else return false;
}
// Handles the collision of the alien spaceship
void AlienSpaceship::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}