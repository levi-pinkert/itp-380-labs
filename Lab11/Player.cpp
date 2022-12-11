#include "Player.h"
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "PlayerMove.h"
#include "Random.h"
#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "Arrow.h"

Player::Player(Game* game, Actor* parent) : Actor(game, parent) {
	//Create movement components
	moveComp = new PlayerMove(this);

	//Create camera component
	cameraComp = new CameraComponent(this);

	//Create collision component
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(COLL_WIDTH, COLL_HEIGHT, COLL_DEPTH);

	//Create the arrow
	new Arrow(mGame, nullptr);
}

void Player::OnUpdate(float deltaTime) {
	
}

void Player::Die() {
	moveComp->Respawn();
}
