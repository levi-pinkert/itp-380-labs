#include "Player.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "CollisionComponent.h"
#include "PlayerMove.h"

Player::Player(Game* game) : Actor(game) {
	//Instantiate collision and move components
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(COLLISION_SIZE, COLLISION_SIZE);
	moveComp = new PlayerMove(this);

	//Instantiate the animated sprite component and load the animations
	spriteComp = new AnimatedSprite(this, 150);
	spriteComp->LoadAnimations("Assets/Link");
	spriteComp->SetAnimation("StandDown");

	//Update game's player pointer
	game->SetPlayer(this);
}

Player::~Player() {
	//Update game's player pointer
	mGame->SetPlayer(nullptr);
}
