#include "Asteroid.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Random.h"
#include "Math.h"

Asteroid::Asteroid(Game* game) : Actor(game) {
	moveComp = new MoveComponent(this);
	moveComp->SetForwardSpeed(SPEED);
	spriteComp = new SpriteComponent(this);
	spriteComp->SetTexture(game->GetTexture("Assets/Asteroid.png"));

	//Randomize position and rotation
	mRotation = Random::GetFloatRange(0.0f, Math::TwoPi);
	mPosition = Random::GetVector(Vector2(0.0f, 0.0f), Vector2((float)game->WIDTH, (float)game->HEIGHT));

	//Add to game's list of all asteroids
	game->AddAsteroid(this);
}

Asteroid::~Asteroid() {
	//Remove from game's list of asteroids
	mGame->RemoveAsteroid(this);
}


void Asteroid::OnUpdate(float deltaTime) {
	if (mPosition.x < 0) {
		mPosition.x = (float)mGame->WIDTH;
	}
	else if (mPosition.x > (float)mGame->WIDTH) {
		mPosition.x = 0;
	}

	if (mPosition.y < 0) {
		mPosition.y = (float)mGame->HEIGHT;
	}
	else if (mPosition.y > (float)mGame->HEIGHT) {
		mPosition.y = 0;
	}
}