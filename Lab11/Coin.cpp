#include "Coin.h"
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Player.h"
#include "Actor.h"

Coin::Coin(Game* game, Actor* owner) : Actor(game, owner) {
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(SIZE, SIZE, SIZE);
	MeshComponent* meshComp = new MeshComponent(this);
	meshComp->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Coin.gpmesh"));
}

void Coin::OnUpdate(float deltaTime) {
	//Rotate
	SetRotation(GetRotation() + ROT_SPD * deltaTime);

	//Check player collision
	if (collisionComp->Intersect(mGame->GetPlayer()->GetComponent<CollisionComponent>())) {
		SetState(ActorState::Destroy);
	}
}