#include "Laser.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Random.h"
#include "Math.h"
#include <vector>
#include "Asteroid.h"

Laser::Laser(Game* game) : Actor(game) {
	moveComp = new MoveComponent(this);
	moveComp->SetForwardSpeed(SPEED);
	spriteComp = new SpriteComponent(this);
	spriteComp->SetTexture(game->GetTexture("Assets/Laser.png"));
	lifetime = 0.0f;
}


void Laser::OnUpdate(float deltaTime) {
	//Collide with asteroids
	std::vector<Asteroid*>& asteroids = mGame->GetAsteroids();
	for (Asteroid* asteroid : asteroids) {
		if (Vector2::Distance(mPosition, asteroid->GetPosition()) < 70.0f) {
			mState = ActorState::Destroy;
			asteroid->SetState(ActorState::Destroy);
			break;
		}
	}


	//Keep track of lifetime and destroy if old
	lifetime += deltaTime;
	if (lifetime >= MAX_LIFETIME) {
		mState = ActorState::Destroy;
	}
}