#include "LaserMine.h"
#include "Game.h"
#include "Actor.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "LaserComponent.h"

LaserMine::LaserMine(Game* game, Actor* parent) : Actor(game, parent) {
	MeshComponent* meshComp = new MeshComponent(this);
	meshComp->SetMesh(mGame->GetRenderer()->GetMesh("Assets/LaserMine.gpmesh"));
	new LaserComponent(this);
}