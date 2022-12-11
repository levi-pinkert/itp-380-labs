#include "EnemyComponent.h"
#include "Game.h"
#include "Actor.h"
#include "Component.h"
#include "CollisionComponent.h"
#include <vector>
#include "Math.h"

EnemyComponent::EnemyComponent(Actor* owner) : Component(owner) {
	//Initialize member variables
	hitPoints = 0;
	invincibilityTimer = 0.0f;
	collisionComp = owner->GetComponent<CollisionComponent>();
	onDamageCallback = nullptr;
	onDeathCallback = nullptr;

	//Add this to the game's list of EnemyComponents
	owner->GetGame()->GetEnemies().push_back(this);
}

EnemyComponent::~EnemyComponent() {
	//Remove this from the game's list of EnemyComponents
	std::vector<EnemyComponent*>& enemyComps = mOwner->GetGame()->GetEnemies();
	auto it = enemyComps.begin();
	while (*it != this) { ++it; }
	enemyComps.erase(it);
}

CollisionComponent* EnemyComponent::GetCollisionComp() {
	return collisionComp;
}

int EnemyComponent::GetHitPoints() {
	return hitPoints;
}

void EnemyComponent::SetHitPoints(int newHitPoints) {
	hitPoints = newHitPoints;
}

void EnemyComponent::Update(float deltaTime) {
	//Decrement invincibility timer
	invincibilityTimer = Math::Max(0.0f, invincibilityTimer - deltaTime);
}

void EnemyComponent::TakeDamage() {
	//Only take damage if invincibility timer is done
	if (invincibilityTimer == 0.0f) {
		//Reset invincibility timer and decrement hit points
		invincibilityTimer = INVINCIBILITY_TIME;
		hitPoints--;

		//Call death or damage callback
		if (hitPoints <= 0) {
			if (onDeathCallback != nullptr) {
				onDeathCallback();
			}
			mOwner->SetState(ActorState::Destroy);
		}
		else if(onDamageCallback != nullptr) {
			onDamageCallback();
		}
	}
}