#include "PlayerMove.h"
#include "Actor.h"
#include "VehicleMove.h"
#include "Math.h"
#include "SDL2/SDL.h"
#include "Game.h"
#include "Renderer.h"
#include "Random.h"
#include "Player.h"
#include "HeightMap.h"
#include "PlayerUI.h"
#include "EnemyMove.h"
#include "Enemy.h"

PlayerMove::PlayerMove(Actor* actor) : VehicleMove(actor) {
	//Initialize member variables
	player = static_cast<Player*>(actor);

	//Move to correct starting spot
	mOwner->SetPosition(mOwner->GetGame()->GetHeightMap()->CellToWorld(START_CELLROW, START_CELLCOL));
}

void PlayerMove::ProcessInput(const Uint8* keyState) {
	//Update gas input
	SetGasInput(keyState[SDL_SCANCODE_W]);

	//Update steering input
	if (keyState[SDL_SCANCODE_D] && !keyState[SDL_SCANCODE_A]) {
		SetSteeringInput(SteerDir::RIGHT);
	}
	else if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D]) {
		SetSteeringInput(SteerDir::LEFT);
	}
	else {
		SetSteeringInput(SteerDir::NONE);
	}
	
}

void PlayerMove::Update(float deltaTime) {
	//Run the vehicle's movement
	VehicleMove::Update(deltaTime);
}

void PlayerMove::OnLapChange(int newLap) {
	PlayerUI* playerUI = player->GetComponent<PlayerUI>();
	if (GetLap() >= MAX_LAPS) {
		//Determine whether we won
		Enemy* enemy = mOwner->GetGame()->GetEnemy();
		int enemyLap = enemy->GetComponent<EnemyMove>()->GetLap();
		bool won = GetLap() > enemyLap;
		playerUI->SetRaceState(won ? PlayerUI::Won : PlayerUI::Lost);
		mOwner->GetGame()->EndMusic(won);
		mOwner->SetState(ActorState::Paused);
		enemy->SetState(ActorState::Paused);
	}
	else {
		//Display the new lap in the UI
		playerUI->OnLapChange(newLap);

		//If we just started the last lap, change the music
		if (GetLap() == MAX_LAPS - 1) {
			mOwner->GetGame()->StartFinalLap();
		}
	}
}