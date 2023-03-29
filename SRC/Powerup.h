#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "GameObject.h"

class Powerup : public GameObject
{
public:
	Powerup(char const* const powerupType);
	~Powerup(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
protected:
};

#endif
