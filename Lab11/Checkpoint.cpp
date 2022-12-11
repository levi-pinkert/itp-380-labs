#include "Checkpoint.h"
#include "Actor.h"
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Player.h"

Checkpoint::Checkpoint(Game* game, Actor* parent) : Actor(game, parent) {
	meshComp = new MeshComponent(this);
	meshComp->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Checkpoint.gpmesh"));
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(SIZE, SIZE, SIZE);
}

void Checkpoint::SetActive(bool newActive) {
	active = newActive;
}

bool Checkpoint::GetActive() {
	return active;
}

void Checkpoint::OnUpdate(float deltaTime) {
	meshComp->SetTextureIndex(active ? 0 : 1);

	//Check for checkpoint
	if (active && mGame->GetPlayer()->GetComponent<CollisionComponent>()->Intersect(collisionComp)) {
		if (!mLevelString.empty()) {
			mGame->SetNextLevel(mLevelString);
		}
		mGame->RemoveCheckpoint();
	}
}