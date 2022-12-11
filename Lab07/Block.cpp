#include "Block.h"
#include "MeshComponent.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "Math.h"
#include <vector>
#include "Player.h"

Block::Block(Game* game, bool exp) : Actor(game), explosive(exp) {
	//Increase scale
	SetScale(SCALE);

	//Set Mesh
	MeshComponent* mesh = new MeshComponent(this);
	mesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	mesh->SetTextureIndex(explosive ? 4 : 3);

	//Set Collision
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(1.0f, 1.0f, 1.0f);

	//Add to game's list
	mGame->GetBlocks().push_back(this);
}

Block::~Block() {
	//Remove from game's list
	std::vector<Block*>& blocks = mGame->GetBlocks();
	auto it = blocks.begin();
	while (*it != this) { ++it; }
	blocks.erase(it);
}

void Block::OnUpdate(float deltaTime) {
	float xDist = mGame->GetPlayer()->GetPosition().x - GetPosition().x;
	if (xDist > DESTROY_DIST) {
		SetState(ActorState::Destroy);
	}
}

CollisionComponent* Block::GetCollision() {
	return collisionComp;
}

void Block::Explode() {
	//If the block is already dead or not explosive, don't do anything
	if (GetState() == ActorState::Destroy) { return; }

	//Destroy this block
	SetState(ActorState::Destroy);
	
	//If this isn't explosive, don't explode nearby blocks
	if (!explosive) { return; }

	//Explode nearby blocks
	std::vector<Block*>& blocks = mGame->GetBlocks();
	for (Block* block : blocks) {
		if (Vector3::Distance(GetPosition(), block->GetPosition()) < EXPLOSION_RADIUS) {
			block->Explode();
		}
	}
}

bool Block::IsExplosive() {
	return explosive;
}