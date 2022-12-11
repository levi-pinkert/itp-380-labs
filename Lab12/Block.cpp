#include "Block.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Game.h"
#include <vector>
#include "Renderer.h"
#include "Mesh.h"

Block::Block(Game* game, Actor* parent) : Actor(game, parent) {
	//Create mesh
	meshComp = new MeshComponent(this);
	meshComp->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));

	//Create collision
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(1.0f, 1.0f, 1.0f);

	//Set scale
	SetScale(SCALE);

	//Add to game's list
	mGame->GetBlocks().push_back(this);
}

Block::~Block() {
	//Remove self from game's list
	std::vector<Actor*>& blocks = mGame->GetBlocks();
	for (size_t i = 0; i < blocks.size(); i++) {
		if (blocks[i] == this) {
			blocks.erase(blocks.begin() + i);
			break;
		}
	}
}

void Block::OnUpdate(float deltaTime) {
	if (rotates) {
		SetRotation(GetRotation() + ROT_SPD * deltaTime);
	}
}