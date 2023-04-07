#include <stdlib.h>
#include "GameUtil.h"
#include "Shield.h"
#include "BoundingShape.h"
#include "Spaceship.h"

// This is the class for the shield that stays in the same position as the spaceship
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
	// Shield when hit by asteroid, alien spaceship or alien bullet
	if (o->GetType() == GameObjectType("Asteroid") || o->GetType() == GameObjectType("AlienSpaceship") || o->GetType() == GameObjectType("AlienBullet")) {
		// The player will not lose the shield if there is a collision at the begining of a level and they are invincible
		if (!mSpaceship->IsInvincible()) {
			return mBoundingShape->CollisionTest(o->GetBoundingShape());
		}
		return false;
	}
	else return false;
}
// Handles the collision of the shield
void Shield::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}
// Updates the position of the shield to stay with the player
void Shield::Update(int t)
{
	SetPosition(mSpaceship->GetPosition());
}
