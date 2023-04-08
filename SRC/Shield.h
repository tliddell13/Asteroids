#ifndef __SHIELD_H__
#define __SHIELD_H__

#include "GameObject.h"
#include "Spaceship.h"

class Shield : public GameObject
{
public:
	Shield(const shared_ptr<Spaceship>& spaceship);
	~Shield(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
    void Update(int t);
	int getHits(void);

private:
	shared_ptr<Spaceship> mSpaceship;
};

#endif
