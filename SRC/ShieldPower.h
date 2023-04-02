#ifndef __SHIELDPOWER_H__
#define __SHIELDPOWER_H__

#include "GameObject.h"

class ShieldPower : public GameObject
{
public:
	ShieldPower(void);
	~ShieldPower(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif
