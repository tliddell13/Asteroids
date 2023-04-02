#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "Spaceship.h"
#include "BoundingSphere.h"

using namespace std;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/**  Default constructor. */
Spaceship::Spaceship()
	: GameObject("Spaceship"), mThrust(0)
{
}

/** Construct a spaceship with given position, velocity, acceleration, angle, and rotation. */
Spaceship::Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("Spaceship", p, v, a, h, r), mThrust(0)
{
}

/** Copy constructor. */
Spaceship::Spaceship(const Spaceship& s)
	: GameObject(s), mThrust(0)
{
}

/** Destructor. */
Spaceship::~Spaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update this spaceship. */
void Spaceship::Update(int t)
{
	// Call parent update function
	GameObject::Update(t);
}

/** Render this spaceship. */
void Spaceship::Render(void)
{
	if (mSpaceshipShape.get() != NULL) mSpaceshipShape->Render();

	// If ship is thrusting
	if ((mThrust > 0) && (mThrusterShape.get() != NULL)) {
		mThrusterShape->Render();
	}

	GameObject::Render();
}

/** Fire the rockets. */
void Spaceship::Thrust(float t)
{
	mThrust = t;
	// Increase acceleration in the direction of ship
	mAcceleration.x = mThrust*cos(DEG2RAD*mAngle);
	mAcceleration.y = mThrust*sin(DEG2RAD*mAngle);
}

/** Set the rotation. */
void Spaceship::Rotate(float r)
{
	mRotation = r;
}

/** Shoot a bullet. */
void Spaceship::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;
	// Construct a unit length vector in the direction the spaceship is headed
	GLVector3f spaceship_heading(cos(DEG2RAD*(mAngle)), sin(DEG2RAD*(mAngle)), 0);
	spaceship_heading.normalize();
	// Calculate the point at the node of the spaceship from position and heading
	GLVector3f bullet_position = mPosition + (spaceship_heading * 4);
	// Calculate how fast the bullet should travel
	float bullet_speed = 30;
	// Construct a vector for the bullet's velocity
	GLVector3f bullet_velocity = mVelocity + spaceship_heading * bullet_speed;
	// Construct a new bullet
	shared_ptr<GameObject> bullet
		(new Bullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mBulletShape);
	// Add the new bullet to the game world
	mWorld->AddObject(bullet);

}

/** Powerup Triple bullet */
void Spaceship::TripleShot(void)
{
	// Check the world exists
	if (!mWorld) return;

	// Construct a unit length vector in the direction the spaceship is headed
	// Heading Center
	GLVector3f spaceship_headingCenter(cos(DEG2RAD * (mAngle)), sin(DEG2RAD * (mAngle)), 0);
	spaceship_headingCenter .normalize();
	// Heading Right
	GLVector3f spaceship_headingRight(cos(DEG2RAD * (mAngle + 30)), sin(DEG2RAD * (mAngle + 30)), 0);
	spaceship_headingRight.normalize();
	// Headign Left
	GLVector3f spaceship_headingLeft(cos(DEG2RAD * (mAngle - 30)), sin(DEG2RAD * (mAngle - 30)), 0);
	spaceship_headingLeft.normalize();

	// Calculate the point at the node of the spaceship from position and heading
	// Center position
	GLVector3f bullet_positionCenter = mPosition + (spaceship_headingCenter * 4);
	// Right position
	GLVector3f bullet_positionRight = mPosition + (spaceship_headingRight * 4);
	// Left position
	GLVector3f bullet_positionLeft = mPosition + (spaceship_headingLeft * 4);
	// Calculate how fast the bullet should travel
	float bullet_speed = 30;
	// Construct a vector for the bullets velocity
	// Center velocity
	GLVector3f bullet_velocityCenter = mVelocity + spaceship_headingCenter * bullet_speed;
	// Right velocity
	GLVector3f bullet_velocityRight = mVelocity + spaceship_headingRight * bullet_speed;
	// Left velocity
	GLVector3f bullet_velocityLeft = mVelocity + spaceship_headingLeft * bullet_speed;
	// Construct the new bullets and add them to the game world
	// Center bullet
	shared_ptr<GameObject> bulletCenter
	(new Bullet(bullet_positionCenter, bullet_velocityCenter, mAcceleration, mAngle, 0, 2000));
	bulletCenter->SetBoundingShape(make_shared<BoundingSphere>(bulletCenter->GetThisPtr(), 2.0f));
	bulletCenter->SetShape(mBulletShape);
	// Right bullet
	shared_ptr<GameObject> bulletRight
	(new Bullet(bullet_positionRight, bullet_velocityRight, mAcceleration, mAngle + 30, 0, 2000));
	bulletRight->SetBoundingShape(make_shared<BoundingSphere>(bulletRight->GetThisPtr(), 2.0f));
	bulletRight->SetShape(mBulletShape);
	// Left bullet
	shared_ptr<GameObject> bulletLeft
	(new Bullet(bullet_positionLeft, bullet_velocityLeft, mAcceleration, mAngle - 30, 0, 2000));
	bulletLeft->SetBoundingShape(make_shared<BoundingSphere>(bulletLeft->GetThisPtr(), 2.0f));
	bulletLeft->SetShape(mBulletShape);
	// Add the new bullet to the game world
	mWorld->AddObject(bulletCenter);
	mWorld->AddObject(bulletRight);
	mWorld->AddObject(bulletLeft);
}

void Spaceship::TurnShieldOn(bool s) {
	mShieldOn = s;
}

void Spaceship::SetInvincible(bool i) {
	mInvincible = i;
}

bool Spaceship::IsInvincible(void) {
	return mInvincible;
}

bool Spaceship::CollisionTest(shared_ptr<GameObject> o)
{
	// Check if the object collided with is a powerup
	if (o->GetType() != GameObjectType("Asteroid") || mShieldOn) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Spaceship::OnCollision(const GameObjectList &objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}

