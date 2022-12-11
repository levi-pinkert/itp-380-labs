#include "PlayerMove.h"
#include "Actor.h"
#include "SDL2/SDL.h"
#include "Math.h"
#include <algorithm>
#include "Game.h"
#include <vector>
#include "CollisionComponent.h"
#include "SpriteComponent.h"
#include "AnimatedSprite.h"
#include "AnimatedSprite.h"
#include "Collider.h"
#include "EnemyComponent.h"
#include "Sword.h"
#include "SDL2/SDL_mixer.h"

PlayerMove::PlayerMove(Actor* actor) : MoveComponent(actor) {
	//Initialize member variables
	moveDirName = None;
	facingDir = Down;

	//Create sword
	sword = new Sword(mOwner->GetGame());
}

void PlayerMove::ProcessInput(const Uint8* keyState) {
	//Set move direction based on arrow key input
	if (keyState[SDL_SCANCODE_RIGHT]) {
		moveDir = Vector2::UnitX;
		moveDirName = MoveDir::Right;
	}
	else if (keyState[SDL_SCANCODE_UP]) {
		moveDir = Vector2::NegUnitY;
		moveDirName = MoveDir::Up;
	}
	else if (keyState[SDL_SCANCODE_LEFT]) {
		moveDir = Vector2::NegUnitX;
		moveDirName = MoveDir::Left;
	}
	else if (keyState[SDL_SCANCODE_DOWN]) {
		moveDir = Vector2::UnitY;
		moveDirName = MoveDir::Down;
	}
	else {
		moveDir = Vector2::Zero;
		moveDirName = MoveDir::None;
	}

	//Check for starting an attack
	if (!lastAttackInput && attackTimer == 0.0f && keyState[SDL_SCANCODE_SPACE]) {
		//If there was a new movement input this frame, take it into account before locking movement and creating sword
		if (moveDirName != MoveDir::None) { facingDir = moveDirName; }

		//Start the attack
		attackTimer = ATTACK_LEN;
		mOwner->GetComponent<AnimatedSprite>()->ResetAnimTimer();
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/SwordSlash.wav"), 0);
	}
	lastAttackInput = keyState[SDL_SCANCODE_SPACE];
}

void PlayerMove::UpdateSword() {
	//Move the sword into the right position based on facing direction
	Vector2 swordOffset = Vector2::Zero, swordSize = Vector2::Zero;
	if (facingDir == Up || facingDir == Down) {
		swordSize = Vector2(SWORD_SIZE_SHORT, SWORD_SIZE_LONG);
		swordOffset.y = facingDir == Up ? -SWORD_DIST_VERTICAL : SWORD_DIST_VERTICAL;
	}
	else {
		swordSize = Vector2(SWORD_SIZE_LONG, SWORD_SIZE_SHORT);
		swordOffset.x = facingDir == Left ? -SWORD_DIST_HORIZONTAL : SWORD_DIST_HORIZONTAL;
	}
	sword->GetCollisionComp()->SetSize(swordSize.x, swordSize.y);
	sword->SetPosition(mOwner->GetPosition() + swordOffset);
}

void PlayerMove::Update(float deltaTime) {
	//Update attack status
	attackTimer = Math::Max(attackTimer - deltaTime, 0.0f);

	//Move the player according to input
	if (attackTimer == 0.0f) {
		mOwner->SetPosition(mOwner->GetPosition() + moveDir * MOVE_SPEED * deltaTime);
	}

	//Move the sword
	UpdateSword();

	//Check for killing and colliding with enemies
	const std::vector<EnemyComponent*> enemyComps = mOwner->GetGame()->GetEnemies();
	CollisionComponent* collisionComp = mOwner->GetComponent<CollisionComponent>();
	Vector2 offset = Vector2::Zero;
	for (EnemyComponent* enemyComp : enemyComps) {
		if (attackTimer > 0.0f && sword->GetCollisionComp()->Intersect(enemyComp->GetCollisionComp())) {
			enemyComp->TakeDamage();
		}
		if (collisionComp->GetMinOverlap(enemyComp->GetCollisionComp(), offset) != CollSide::None) {
			mOwner->SetPosition(mOwner->GetPosition() + offset);
		}
	}

	//Resolve any collisions with colliders
	const std::vector<Collider*> colliders = mOwner->GetGame()->GetColliders();
	for (Collider* collider : colliders) {
		if (collisionComp->GetMinOverlap(collider->getCollisionComp(), offset) != CollSide::None) {
			mOwner->SetPosition(mOwner->GetPosition() + offset);
		}
	}

	//Set the camera position to center on the player
	mOwner->GetGame()->GetCameraPos() = mOwner->GetPosition() + Vector2(CAMERA_OFFSETX, CAMERA_OFFSETY);

	//Update the player's animation
	const std::string* animSet = attackTimer > 0.0f ? attackAnims : (moveDirName == None ? standAnims : walkAnims);
	if (moveDirName != None && attackTimer == 0.0f) {
		facingDir = moveDirName;
	}
	mOwner->GetComponent<AnimatedSprite>()->SetAnimation(animSet[facingDir]);
}


