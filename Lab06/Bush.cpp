#include "Bush.h"
#include "Game.h"
#include "Actor.h"
#include "CollisionComponent.h"
#include "SpriteComponent.h"
#include "EnemyComponent.h"
#include "PathFinder.h"
#include "Effect.h"

Bush::Bush(Game* game) : Actor(game) {
	//Make sprite and collision components
	spriteComp = new SpriteComponent(this);
	spriteComp->SetTexture(game->GetTexture("Assets/Bush.png"));
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(32.0f, 32.0f);

	//Make this an enemy so that the player can hit it
	enemyComp = new EnemyComponent(this);
	enemyComp->SetHitPoints(1);
	enemyComp->SetOnDeath([this]() {
		//Clear the way in the pathfinder graph
		mGame->GetPathFinder()->SetIsBlocked((size_t)(mPosition.y / 32.0f), (size_t)(mPosition.x / 32.0f), false);

		//Play an effect
		new Effect(mGame, mPosition, "BushDeath", "Assets/Sounds/BushDie.wav");
	});
}