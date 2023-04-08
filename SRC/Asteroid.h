#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"
#include "Spaceship.h"

class Asteroid : public GameObject
{
public:
	Asteroid(const shared_ptr<Spaceship> spaceship);
	~Asteroid(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
private:
	shared_ptr<Spaceship> mSpaceship;
};

#endif
