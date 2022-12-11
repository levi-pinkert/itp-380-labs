#include "Frog.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Math.h"
#include "CollisionComponent.h"
#include <vector>
#include "Vehicle.h"
#include "DeadFrog.h"
#include "Log.h"
#include "WrappingMove.h"

Frog::Frog(Game* game) : Actor(game) {
	//Create sprite and collision component
	spriteComp = new SpriteComponent(this);
	spriteComp->SetTexture(game->GetTexture("Assets/Frog.png"));
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(50.0f, 50.0f);

	//Initialize all last frame's inputs to false
	lastFrameInput[SDL_SCANCODE_RIGHT] = false;
	lastFrameInput[SDL_SCANCODE_UP] = false;
	lastFrameInput[SDL_SCANCODE_LEFT] = false;
	lastFrameInput[SDL_SCANCODE_DOWN] = false;

	//Set the game's frog pointer to this
	game->SetFrog(this);
}

Frog::~Frog() {
	//Remove this from the game's frog pointer
	mGame->SetFrog(nullptr);
}

void Frog::OnProcessInput(const Uint8* keyState) {
	//Detect leading edges and move accordingly
	if (keyState[SDL_SCANCODE_RIGHT] && !lastFrameInput[SDL_SCANCODE_RIGHT]) {
		mPosition.x += MOVE_DIST;
	}
	if (keyState[SDL_SCANCODE_UP] && !lastFrameInput[SDL_SCANCODE_UP]) {
		mPosition.y -= MOVE_DIST;
	}
	if (keyState[SDL_SCANCODE_LEFT] && !lastFrameInput[SDL_SCANCODE_LEFT]) {
		mPosition.x -= MOVE_DIST;
	}
	if (keyState[SDL_SCANCODE_DOWN] && !lastFrameInput[SDL_SCANCODE_DOWN]) {
		mPosition.y += MOVE_DIST;
	}

	//Limit movement to bounds of screen
	mPosition.x = Math::Clamp(mPosition.x, 0.0f, (float)mGame->WIDTH);
	mPosition.y = Math::Clamp(mPosition.y, 0.0f, (float)mGame->HEIGHT);

	//Save inputs for next frame
	lastFrameInput[SDL_SCANCODE_RIGHT] = keyState[SDL_SCANCODE_RIGHT];
	lastFrameInput[SDL_SCANCODE_UP] = keyState[SDL_SCANCODE_UP];
	lastFrameInput[SDL_SCANCODE_LEFT] = keyState[SDL_SCANCODE_LEFT];
	lastFrameInput[SDL_SCANCODE_DOWN] = keyState[SDL_SCANCODE_DOWN];
}

void Frog::OnUpdate(float deltaTime) {
	//Check for collision with a car
	std::vector<Vehicle*>& vehicles = mGame->GetVehicles();
	for (Vehicle* vehicle : vehicles) {
		if (vehicle->GetCollisionComponent()->Intersect(collisionComp)) {
			//The frog got hit by a car!
			Die();
		}
	}

	//Check if the frog is riding a log
	std::vector<Log*>& logs = mGame->GetLogs();
	bool ridingLog = false;
	for (Log* log : logs) {
		Vector2 minOverlap;
		CollSide collSide = collisionComp->GetMinOverlap(log->GetCollisionComponent(), minOverlap);
		if (collSide != CollSide::None) {
			//Follow the log's motion
			mPosition.y = log->GetPosition().y;
			WrappingMove* logMoveComp = log->GetWrappingMoveComponent();
			mPosition.x += logMoveComp->GetMoveDirection().x * logMoveComp->GetForwardSpeed() * deltaTime;

			//Move horizontally onto the log if necessary
			if (collSide == CollSide::Left) {
				mPosition.x += LOG_X_ADJUST;
			}else if (collSide == CollSide::Right) {
				mPosition.x -= LOG_X_ADJUST;
			}

			//Remember that we're on a log this frame
			ridingLog = true;
		}
	}

	//Die if in the water and not riding a log
	if (mPosition.y >= WATER_TOP && mPosition.y <= WATER_BOTTOM && !ridingLog) {
		Die();
	}

	//Check if reached goal
	if (collisionComp->Intersect(mGame->GetGoal()->GetComponent<CollisionComponent>())) {
		//Player wins!
		SetPosition(mGame->GetGoal()->GetPosition());
		SetState(ActorState::Paused);
	}
	else if (mPosition.y < WATER_TOP) {
		//Player must have reached the last row but not hit the goal
		Die();
	}
}

void Frog::Die() {
	//Create DeadFrog at position
	DeadFrog* roadkill = new DeadFrog(mGame);
	roadkill->SetPosition(mPosition);

	//Respawn
	SetPosition(RESPAWN_POS);
}