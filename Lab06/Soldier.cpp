#include "Soldier.h"
#include "CollisionComponent.h"
#include "AnimatedSprite.h"
#include "Game.h"
#include "SoldierAI.h"
#include "EnemyComponent.h"
#include "Effect.h"

Soldier::Soldier(Game* game, PathNode* start, PathNode* end) : Actor(game) {
	//Create collision and animation components
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(32.0f, 32.0f);
	spriteComp = new AnimatedSprite(this);
	spriteComp->LoadAnimations("Assets/Soldier");
	spriteComp->SetAnimation("WalkDown");
	spriteComp->SetAnimFPS(5.0f);

	//Create AI component
	aiComp = new SoldierAI(this);
	aiComp->Setup(start, end);

	//Create the enemy component
	enemyComp = new EnemyComponent(this);
	enemyComp->SetHitPoints(2);
	enemyComp->SetOnDamage([this]() {
		//Stun (stop movement for a moment)
		aiComp->Stun();

		//Play an effect
		new Effect(mGame, mPosition, "Hit", "Assets/Sounds/EnemyHit.wav");
	});
	enemyComp->SetOnDeath([this]() {
		//Play an effect
		new Effect(mGame, mPosition, "Death", "Assets/Sounds/EnemyDie.wav");
	});
}