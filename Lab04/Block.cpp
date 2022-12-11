#include "Block.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include <string>

Block::Block(Game* game, char type) : Actor(game) {
	//Create sprite and collision component
	spriteComp = new SpriteComponent(this);
	spriteComp->SetTexture(game->GetTexture(std::string("Assets/Block") + std::string(1, type) + std::string(".png")));
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(SIZE, SIZE);

	//Add self to game's list of blocks
	game->AddBlock(this);
}

Block::~Block() {
	//Remove self from game's list of blocks
	mGame->RemoveBlock(this);
}

CollisionComponent* Block::GetCollisionComponent() {
	return collisionComp;
}