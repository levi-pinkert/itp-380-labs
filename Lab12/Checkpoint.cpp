#include "Checkpoint.h"
#include "Actor.h"
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Player.h"
#include "HUD.h"

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
	if (mGame->GetActiveCheckpoint() == this && mGame->GetPlayer()->GetComponent<CollisionComponent>()->Intersect(collisionComp)) {
		Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Checkpoint.wav"), 0);
		mGame->GetPlayer()->GetComponent<HUD>()->SetCheckpointText(mText);
		if (!mLevelString.empty()) {
			mGame->SetNextLevel(mLevelString);
		}
		mGame->RemoveCheckpoint();
	}
}