#include "Asteroid.h"
#include "PointBonus.h"
#include "ExtraLife.h"
#include "BulletUpgrade.h"
#include "ShieldPower.h"
#include "Shield.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "AlienSpaceship.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	// Initialize variables for start of game
	mLevel = 1;
	mAsteroidCount = 0;
	tripleShot = false;
	shieldHealth = 0;
	
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	// The animations for the project
	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *bigAsteroid_anim = AnimationManager::GetInstance().CreateAnimationFromFile("bigAsteroid", 64, 4096, 64, 64, "bigAsteroid.png");
	Animation *mediumAsteroid_anim = AnimationManager::GetInstance().CreateAnimationFromFile("mediumAsteroid", 128, 8192, 128, 128, "mediumAsteroid.png");
	Animation *smallAsteroid_anim = AnimationManager::GetInstance().CreateAnimationFromFile("smallAsteroid", 128, 8192, 128, 128, "smallAsteroid.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");
	Animation *points_anim = AnimationManager::GetInstance().CreateAnimationFromFile("coin", 128, 128, 128, 128, "coin2.png");
	Animation *life_anim = AnimationManager::GetInstance().CreateAnimationFromFile("life", 128, 128, 128, 128, "life.png");
	Animation *shield_anim = AnimationManager::GetInstance().CreateAnimationFromFile("shield", 128, 128, 128, 128, "shield.png");
	Animation *bulletPowerup_anim = AnimationManager::GetInstance().CreateAnimationFromFile("bulletPowerup", 128, 128, 128, 128, "shieldGreen.png");
	Animation *greenShield_anim = AnimationManager::GetInstance().CreateAnimationFromFile("shieldGreen", 128, 128, 128, 128, "shieldGreen.png");
	Animation *yellowShield_anim = AnimationManager::GetInstance().CreateAnimationFromFile("shieldYellow", 128, 128, 128, 128, "shieldYellow.png");
	Animation* alienSpaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("alienSpaceship", 128, 8192, 128, 128, "alienSpaceship.png");
	// Create a spaceship and add it to the world
	mGameWorld->AddObject(CreateSpaceship());
	SetTimer(500, BLINKOFF);
	// Set invincibility timer for beginning of game to shut off invincibility
	SetTimer(3000, REMOVE_INVINCIBILITY);
	// Create some asteroids and add them to the world
	CreateAsteroids(2);

	// Spawn the powerups 
	SpawnPowerups();

	//Create the GUI
	CreateGUI();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	switch (key)
	{
	case ' ':
		if (tripleShot) {
			mSpaceship->TripleShot();
		}
		else mSpaceship->Shoot();
		break;
	default:
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
	// Default case - do nothing
	default: break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// Default case - do nothing
	default: break;
	} 
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		// When the level is past level 1
		// If the asteroid is one of the original sized asteroids split it and place 3 more in the same position
		if (object->GetScale() == 0.5f && mLevel > 1) {
			SplitAsteroids(2, object->GetPosition());
			mAsteroidCount += 2;
		}
		if (object->GetScale() == 0.2f && mLevel > 2) {
			SplitAsteroids(3, object->GetPosition());
			mAsteroidCount += 3;
		}
		if (mAsteroidCount <= 0) 
		{ 
			// Make player invincible for four seconds and start next level in two seconds
			SetTimer(500, BLINKOFF);
			mSpaceship->SetInvincible(true);
			SetTimer(3000, REMOVE_INVINCIBILITY);
			SetTimer(2000, START_NEXT_LEVEL); 

		}
	}
	// If a bullet upgrade is destroyed turn on the triple shot
	if (object->GetType() == GameObjectType("BulletUpgrade"))
	{
		tripleShot = true;
	}
	// If the shield powerup is destroyed add the shield
	if (object->GetType() == GameObjectType("ShieldPower"))
	{
		// Keeps track of this shields health
		if (shieldHealth == 0) {
			shieldHealth = 3;
			AddShield();
		}
	}
	// Add a new shield if there is any shield health remaining
	if (object->GetType() == GameObjectType("Shield"))
	{
		shieldHealth--;
		// Delay the addition of a new shield so it isn't immediatedly deleted
		AddShield();
	}
	// Add an explosion for alien spaceship
	if (object->GetType() == GameObjectType("AlienSpaceship"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(mAlienSpaceship->GetPosition());
		explosion->SetRotation(mAlienSpaceship->GetRotation());
		mGameWorld->AddObject(explosion);
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 2 + mLevel;
		CreateAsteroids(num_asteroids);
		SpawnPowerups();
		
		if (mLevel > 1) {
			mGameWorld->AddObject(CreateAlienSpaceship());
			SetTimer(2000, ALIEN_MOVEMENT);
			SetTimer(2000, ALIEN_SHOOT);
		}
		
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
	}
	// Spawn a powerup after a certain amount of time
	if (value == CREATE_POWERUP)
	{
		srand(time(0));

		// Don't spawn shield or bullet upgrades if the player already has them
		
		int powerupChoice = rand() % 4 + 1;
		// Spawn a point bonus
		if (powerupChoice == 1) {
			shared_ptr<GameObject> pointBonus = make_shared<PointBonus>();
			pointBonus->SetBoundingShape(make_shared<BoundingSphere>(pointBonus->GetThisPtr(), 4.0f));
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("coin");
			shared_ptr<Sprite> points_sprite
				= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			pointBonus->SetSprite(points_sprite);
			pointBonus->SetScale(0.07f);
			mGameWorld->AddObject(pointBonus);
		}
		// Spawn an extra life
		if (powerupChoice == 2) {
			shared_ptr<GameObject> extraLife = make_shared<ExtraLife>();
			extraLife->SetBoundingShape(make_shared<BoundingSphere>(extraLife->GetThisPtr(), 4.0f));
			Animation* anim_ptr2 = AnimationManager::GetInstance().GetAnimationByName("life");
			shared_ptr<Sprite> life_sprite
				= make_shared<Sprite>(anim_ptr2->GetWidth(), anim_ptr2->GetHeight(), anim_ptr2);
			extraLife->SetSprite(life_sprite);
			extraLife->SetScale(0.07f);
			mGameWorld->AddObject(extraLife);
		}
		// Spawn a bullet upgrade
		if (powerupChoice == 3) {
			shared_ptr<GameObject> bulletUpgrade = make_shared<BulletUpgrade>();
			bulletUpgrade->SetBoundingShape(make_shared<BoundingSphere>(bulletUpgrade->GetThisPtr(), 4.0f));
			Animation* anim_ptr3 = AnimationManager::GetInstance().GetAnimationByName("bulletPowerup");
			shared_ptr<Sprite> bullet_sprite
				= make_shared<Sprite>(anim_ptr3->GetWidth(), anim_ptr3->GetHeight(), anim_ptr3);
			bulletUpgrade->SetSprite(bullet_sprite);
			bulletUpgrade->SetScale(0.07f);
			mGameWorld->AddObject(bulletUpgrade);
		}
		
		// Spawn a shield upgrade
		if (powerupChoice == 4) {
			shared_ptr<GameObject> shield = make_shared<ShieldPower>();
			shield->SetBoundingShape(make_shared<BoundingSphere>(shield->GetThisPtr(), 4.0f));
			Animation* anim_ptr4 = AnimationManager::GetInstance().GetAnimationByName("shield");
			shared_ptr<Sprite> shield_sprite
				= make_shared<Sprite>(anim_ptr4->GetWidth(), anim_ptr4->GetHeight(), anim_ptr4);
			shield->SetSprite(shield_sprite);
			shield->SetScale(0.07f);
			mGameWorld->AddObject(shield);
		}
	} 
	// Removes the invincibility when the level is starting
	if (value == REMOVE_INVINCIBILITY) {
		mSpaceship->SetInvincible(false);
	}
	// Makes the spaceship blink at the beginning of the level when it is invincible
	if (value == BLINK) {
		mSpaceship->SetSprite(mSpaceship_sprite);
		if (mSpaceship->IsInvincible()) {
			SetTimer(800, BLINKOFF);
		}
	}
	if (value == BLINKOFF) {
		mSpaceship->SetSprite(NULL);
		SetTimer(600, BLINK);
	}
	if (value == SHIELD_DELAY) {
		AddShield();
	}
	// The timer to keep the alien spaceship moving
	if (value == ALIEN_MOVEMENT) {
		if (mAlienSpaceship->GetThrust() == 0) {
			mAlienSpaceship->Thrust(8);
		}
		else {
			mAlienSpaceship->Thrust(0);
		}
		SetTimer(100, ALIEN_MOVEMENT);
	}
	// The timer for the alien shooting
	if (value == ALIEN_SHOOT) {
		mAlienSpaceship->Shoot();
		SetTimer(2000, ALIEN_SHOOT);
	}

}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 5.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	mSpaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(mSpaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	mSpaceship->SetInvincible(true);
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

shared_ptr<GameObject> Asteroids::CreateAlienSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mAlienSpaceship = make_shared<AlienSpaceship>(mSpaceship);
	mAlienSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mAlienSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("alienBullet.shape");
	mAlienSpaceship->SetBulletShape(bullet_shape);
	Animation *alienAnim_ptr = AnimationManager::GetInstance().GetAnimationByName("alienSpaceship");
	mAlienSpaceship_sprite =
		make_shared<Sprite>(alienAnim_ptr->GetWidth(), alienAnim_ptr->GetHeight(), alienAnim_ptr);
	mAlienSpaceship->SetSprite(mAlienSpaceship_sprite);
	mAlienSpaceship->SetScale(0.1f);
	// Return the spaceship so it can be added to the world
	return mAlienSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("bigAsteroid");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>(mSpaceship);
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 12.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.5f);
		mGameWorld->AddObject(asteroid);
	}
}
// This function is exactly like create asteroids but it splits the asteroid in the location it was
// destroyed at by taking it's position as a parameter.
void Asteroids::SplitAsteroids(const uint num_asteroids, GLVector3f pos)
{
	for (uint i = 0; i < num_asteroids; i++)
	{
		// The image is chosen based on how many asteroids the bigger one is splitting into
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("smallAsteroid");
		if (num_asteroids == 2) {
			anim_ptr = AnimationManager::GetInstance().GetAnimationByName("mediumAsteroid");
		}
		shared_ptr<Sprite> medAsteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		medAsteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>(mSpaceship);
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(medAsteroid_sprite);
		// Medium asteroid size
		asteroid->SetScale(0.2f);
		// Small asteroid size
		if (num_asteroids > 2) {
			asteroid->SetScale(0.1f);
		}
		asteroid->SetPosition(pos);
		mGameWorld->AddObject(asteroid);
	}
}
// Add a shield to the players spaceship
void Asteroids::AddShield() {
	// If shield health is full give the blue shield
	if (shieldHealth >= 3) {
		shared_ptr<GameObject> shield = make_shared<Shield>(mSpaceship);
		shield->SetBoundingShape(make_shared<BoundingSphere>(shield->GetThisPtr(), 18.0f));
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("shield");
		shared_ptr<Sprite> shields_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		shield->SetSprite(shields_sprite);
		shield->SetScale(0.2f);
		mGameWorld->AddObject(shield);
		mSpaceship->TurnShieldOn(true);
	}
	// If this is the first hit give a green shield
	if (shieldHealth == 2) {
		shared_ptr<GameObject> shield = make_shared<Shield>(mSpaceship);
		shield->SetBoundingShape(make_shared<BoundingSphere>(shield->GetThisPtr(), 18.0f));
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("shieldGreen");
		shared_ptr<Sprite> shields_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		shield->SetSprite(shields_sprite);
		shield->SetScale(0.2f);
		mGameWorld->AddObject(shield);
	}
	// If this is the second hit give a yellow shield
	if (shieldHealth == 1) {
		shared_ptr<GameObject> shield = make_shared<Shield>(mSpaceship);
		shield->SetBoundingShape(make_shared<BoundingSphere>(shield->GetThisPtr(), 18.0f));
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("shieldYellow");
		shared_ptr<Sprite> shields_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		shield->SetSprite(shields_sprite);
		shield->SetScale(0.2f);
 		mGameWorld->AddObject(shield);
	}
	else {
		// If no more shield hits remaining
		mSpaceship->TurnShieldOn(false);
	}
}

void Asteroids::SpawnPowerups() {
	// Seed the random number gen
	srand(time(NULL));
	// Pick random times to spawn three powerups in between the times of 5 seconds and 25 seconds
	int randomTime1 = rand() % 25001 + 5000;
	int randomTime2 = rand() % 25001 + 5000;
	int randomTime3 = rand() % 25001 + 5000;
	SetTimer(randomTime1, CREATE_POWERUP);
	SetTimer(randomTime2, CREATE_POWERUP);
	SetTimer(randomTime3, CREATE_POWERUP);
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

// updates the GUI when an extra life is gained
void Asteroids::OnLivesChanged(int lives)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives;
	// Get the score message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
		SetTimer(500, BLINKOFF);
		mSpaceship->SetInvincible(true);
		// Set invincibility timer for beginning of game to shut off invincibility
		SetTimer(3000, REMOVE_INVINCIBILITY);
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}




