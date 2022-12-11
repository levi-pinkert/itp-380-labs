#include "Bullet.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "MoveComponent.h"
#include "Renderer.h"
#include "Block.h"
#include <vector>

Bullet::Bullet(Game* game, float speedMultiplier) : Actor(game) {
	//Make mesh
	SetScale(SCALE);
	MeshComponent* mesh = new MeshComponent(this);
	mesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Laser.gpmesh"));
	
	//Make movement
	MoveComponent* moveComp = new MoveComponent(this);
	moveComp->SetForwardSpeed(SPEED * speedMultiplier);

	//Make collision
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(COLL_SIZE, COLL_SIZE, COLL_SIZE);

	//Start lifetime countdown
	lifetime = 0.0f;
}

void Bullet::OnUpdate(float deltaTime) {
	//Check for lifetime expiry
	lifetime += deltaTime;
	if (lifetime >= MAX_LIFETIME) {
		SetState(ActorState::Destroy);
	}

	//Check for block collision
	std::vector<Block*>& blocks = mGame->GetBlocks();
	for (Block* block : blocks) {
		if (collisionComp->Intersect(block->GetCollision())) {
			//Explode a block and destroy the bullet
			if (block->IsExplosive()) {
				block->Explode();
				Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/BlockExplode.wav"), 0);
			}
			SetState(ActorState::Destroy);
			break;
		}
	}
}