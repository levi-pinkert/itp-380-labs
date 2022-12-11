#include "Sword.h"
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"

Sword::Sword(Game* game) : Actor(game) {
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(28.0f, 28.0f);
}

CollisionComponent* Sword::GetCollisionComp() {
	return collisionComp;
}