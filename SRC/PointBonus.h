#ifndef __POINTBONUS_H__
#define __POINTBONUS_H__

#include "GameObject.h"

class PointBonus : public GameObject
{
public:
	PointBonus(void);
	~PointBonus(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif