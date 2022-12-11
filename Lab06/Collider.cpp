#include "Collider.h"
#include "Game.h"
#include "CollisionComponent.h"

Collider::Collider(Game* game, float width, float height) : Actor(game) {
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(width, height);
}

CollisionComponent* Collider::getCollisionComp() {
	return collisionComp;
}