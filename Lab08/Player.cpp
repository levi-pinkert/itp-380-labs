#include "Player.h"
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "PlayerMove.h"
#include "Random.h"
#include "CameraComponent.h"
#include "PlayerUI.h"

Player::Player(Game* game) : Actor(game) {
	//Set Scale
	SetScale(0.75f);
	
	//Create mesh
	meshComp = new MeshComponent(this);
	meshComp->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Kart.gpmesh"));

	//Create movement components
	moveComp = new PlayerMove(this);

	//Create camera component
	cameraComp = new CameraComponent(this);
	cameraComp->SnapToIdeal();

	//Create UI
	uiComp = new PlayerUI(this);
}

void Player::OnUpdate(float deltaTime) {
	
}
