#include "Goomba.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "CollisionComponent.h"
#include "GoombaMove.h"
#include <vector>

Goomba::Goomba(Game* game) : Actor(game) {
	//Create animations
	spriteComp = new AnimatedSprite(this, 150);
	spriteComp->SetTexture(game->GetTexture("Assets/Goomba/Walk0.png"));
	std::vector<SDL_Texture*> walkAnim{
		mGame->GetTexture("Assets/Goomba/Walk0.png"),
		mGame->GetTexture("Assets/Goomba/Walk1.png")
	};
	spriteComp->AddAnimation("walk", walkAnim);
	std::vector<SDL_Texture*> deadAnim{
		mGame->GetTexture("Assets/Goomba/Dead.png")
	};
	spriteComp->AddAnimation("dead", deadAnim);
	spriteComp->SetAnimation("walk");

	//Instantiate collision and move components
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(SIZE, SIZE);
	moveComp = new GoombaMove(this);
	
	//Add to game's list of goombas
	game->AddGoomba(this);

	//Initialize member variables
	stomped = false;
	despawnTimer = 0.0f;
}

Goomba::~Goomba() {
	//Remove from game's list of goombas
	mGame->RemoveGoomba(this);
}

void Goomba::OnUpdate(float deltaTime)
{
	//If stomped, decrement timer and eventually destroy
	if (stomped) {
		despawnTimer -= deltaTime;
		if (despawnTimer <= 0.0f) {
			SetState(ActorState::Destroy);
		}
	}
}

CollisionComponent* Goomba::GetCollisionComponent() {
	return collisionComp;
}

void Goomba::Stomp() {
	//Don't restart stomp if already stomped
	if (stomped) { return; }

	//Change animation
	spriteComp->SetAnimation("dead");

	//Flag as stomped and start counting down
	stomped = true;
	despawnTimer = DESPAWN_TIME;

	//Stop moveComp from moving
	moveComp->SetCanMove(false);
}

bool Goomba::GetStomped() {
	return stomped;
}