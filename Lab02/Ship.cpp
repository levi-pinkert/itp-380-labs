#include "Ship.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Laser.h"

Ship::Ship(Game* game) : Actor(game) {
	moveComp = new MoveComponent(this);
	spriteComp = new SpriteComponent(this);
	spriteComp->SetTexture(game->GetTexture("Assets/Ship.png"));
	thrustInput = 0;
	steerInput = 0;
	shootInput = false;
	shootCooldown = 0.0f;
}

void Ship::OnProcessInput(const Uint8* keyState) {
	thrustInput = (int)(keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W]) - (int)(keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S]);
	steerInput = (int)(keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) - (int)(keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]);
	shootInput = keyState[SDL_SCANCODE_SPACE];
}

void Ship::OnUpdate(float deltaTime) {
	//Move
	moveComp->SetForwardSpeed(thrustInput * THRUST_SPEED);
	moveComp->SetAngularSpeed(steerInput * STEER_SPEED);

	//Set texture according to thrust input
	spriteComp->SetTexture(mGame->GetTexture(thrustInput != 0 ? "Assets/ShipThrust.png" : "Assets/Ship.png"));

	//Shoot
	shootCooldown -= deltaTime;
	if (shootInput && shootCooldown <= 0.0f) {
		Laser* newLaser = new Laser(mGame);
		newLaser->SetPosition(mPosition);
		newLaser->SetRotation(mRotation);
		shootCooldown = SHOOT_SPEED;
	}
}