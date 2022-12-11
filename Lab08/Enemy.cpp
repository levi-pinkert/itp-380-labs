#include "Enemy.h"
#include "Game.h"
#include "MeshComponent.h"
#include "EnemyMove.h"
#include "Game.h"
#include "Renderer.h"

Enemy::Enemy(Game* game) : Actor(game) {
	//Set scale
	SetScale(0.75f);

	//Instantiate mesh
	MeshComponent* meshComp = new MeshComponent(this);
	meshComp->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Kart.gpmesh"));
	meshComp->SetTextureIndex(6);

	//Instantiate movement comp
	moveComp = new EnemyMove(this);
}