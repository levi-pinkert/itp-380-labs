#include "Vehicle.h"
#include "WrappingMove.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Math.h"
#include "CollisionComponent.h"
#include "Frog.h"

Vehicle::Vehicle(Game* game, char type, Vector2 moveDir) : Actor(game) {
	//Create movement and collision components
	moveComp = new WrappingMove(this, moveDir);
	moveComp->SetForwardSpeed(MOVE_SPEED);
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(64.0f, 64.0f);

	//Set texture (and collision size) based on type
	spriteComp = new SpriteComponent(this);
	switch (type) {
	case 'A':
		spriteComp->SetTexture(game->GetTexture("Assets/CarA.png"));
		break;
	case 'B':
		spriteComp->SetTexture(game->GetTexture("Assets/CarB.png"));
		break;
	case 'C':
		spriteComp->SetTexture(game->GetTexture("Assets/CarC.png"));
		break;
	case 'D':
		spriteComp->SetTexture(game->GetTexture("Assets/CarD.png"));
		break;
	case 'T':
		spriteComp->SetTexture(game->GetTexture("Assets/Truck.png"));
		collisionComp->SetSize(128.0f, 64.0f);
		break;
	}
	
	//Add this to the game's list of vehicles
	game->AddVehicle(this);
}

Vehicle::~Vehicle() {
	//Remove from the game's list of vehicles
	mGame->RemoveVehicle(this);
}


void Vehicle::OnUpdate(float deltaTime) {
	Frog* frog = mGame->GetFrog();
	if (frog != nullptr) {
		Vector2 moveDir = moveComp->GetMoveDirection();
		Vector2 toFrog = frog->GetPosition() - GetPosition();
		float theta = Math::Acos(Vector2::Dot(moveDir, toFrog) / toFrog.Length());
		moveComp->SetForwardSpeed((theta < SLOW_ANGLE) ? (MOVE_SPEED * SLOW_FACTOR) : MOVE_SPEED);
	}
}

CollisionComponent* Vehicle::GetCollisionComponent() {
	return collisionComp;
}