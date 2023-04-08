#ifndef __AlienSpaceship_H__
#define __AlienSpaceship_H__

#include "GameObject.h"
#include "Spaceship.h"

class AlienSpaceship : public GameObject
{
public:
	AlienSpaceship(const shared_ptr<Spaceship> spaceship);
	~AlienSpaceship(void);

	void SetBulletShape(shared_ptr<Shape> bullet_shape) { mBulletShape = bullet_shape; }
	void AlienSpaceship::Shoot(void);
	virtual void Thrust(float t);
	virtual float GetThrust();
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

private:
	float mThrust;

	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<Shape> mBulletShape;
protected:
	// Don't know why but it needs to be here
	bool mEvade;
};

#endif
