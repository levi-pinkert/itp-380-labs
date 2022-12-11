#include "Player.h"
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "PlayerMove.h"
#include "HUD.h"
#include "Random.h"

Player::Player(Game* game) : Actor(game) {
	//Set Scale
	SetScale(2.0f);
	
	//Create mesh
	meshComp = new MeshComponent(this);
	meshComp->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Arwing.gpmesh"));

	//Create collision
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(COLL_X, COLL_Y, COLL_Z);

	//Create move component
	moveComp = new PlayerMove(this);

	//Set Shield Level
	shieldLevel = INIT_SHIELD;

	//Create the HUD
	hud = new HUD(this);
	peppyCooldown = 0.0f;

	//Start sound
	shipLoopChannel = Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/ShipLoop.ogg"), -1);
	damageAlertChannel = -1;
}

CollisionComponent* Player::GetCollisionComponent() {
	return collisionComp;
}

void Player::TakeDamage() {
	shieldLevel--;
	if (shieldLevel <= 0) {
		SetState(ActorState::Paused);
		Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/ShipDie.wav"), 0);
		Mix_HaltChannel(shipLoopChannel);
		if (damageAlertChannel != -1) {
			Mix_HaltChannel(damageAlertChannel);
		}
	}
	else if(shieldLevel == 1) {
		Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/ShipHit.wav"), 0);
		if (damageAlertChannel == -1) {
			damageAlertChannel = Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/DamageAlert.ogg"), -1);
		}
	}
}

void Player::Heal() {
	shieldLevel = std::min(3, shieldLevel + 1);
	if (shieldLevel > 1 && damageAlertChannel != -1) {
		Mix_HaltChannel(damageAlertChannel);
		damageAlertChannel = -1;
	}
}

int Player::GetHitPoints() {
	return shieldLevel;
}

void Player::OnUpdate(float deltaTime) {
	//Make Peppy say "Do a Barrel Roll"
	peppyCooldown = std::max(peppyCooldown - deltaTime, 0.0f);
	if (peppyCooldown == 0.0f && shieldLevel < INIT_SHIELD) {
		hud->DoABarrelRoll();
		peppyCooldown = Random::GetFloatRange(PEPPY_COOLDOWN_MIN, PEPPY_COOLDOWN_MAX);
	}
}