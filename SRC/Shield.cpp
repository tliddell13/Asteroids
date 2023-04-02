#include <stdlib.h>
#include "GameUtil.h"
#include "Shield.h"
#include "BoundingShape.h"
#include "Spaceship.h"

// Change random position to not spawn where the player is at
Shield::Shield(const shared_ptr<Spaceship>& spaceship) : GameObject("Shield"), mSpaceship(spaceship)
{
	SetPosition(mSpaceship->GetPosition());
}

Shield::~Shield(void)
{
}

// Checks for the collision of an asteroid
bool Shield::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	// Shield when hit by asteroid 
	// Shield stays on when player is invincible
	if (o->GetType() == GameObjectType("Asteroid") && !mSpaceship->IsInvincible()) {
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	else return false;
}
// Handles the collision of an asteroid
void Shield::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}

void Shield::Update(int t)
{
	SetPosition(mSpaceship->GetPosition());
}
