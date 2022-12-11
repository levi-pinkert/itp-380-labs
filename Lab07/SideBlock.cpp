#include "SideBlock.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Player.h"

SideBlock::SideBlock(Game* game, size_t textureIndex) : Actor(game) {
	SetScale(500.0f);
	MeshComponent* mesh = new MeshComponent(this);
	mesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	mesh->SetTextureIndex(textureIndex);
}

void SideBlock::OnUpdate(float deltaTime) {
	float xDist = mGame->GetPlayer()->GetPosition().x - GetPosition().x;
	if (xDist > DESTROY_DIST) {
		SetState(ActorState::Destroy);
	}
}