#include "Spawner.h"
#include "Game.h"
#include "Goomba.h"
#include "Player.h"
#include "Math.h"

Spawner::Spawner(Game* game) : Actor(game) {
	
}

void Spawner::OnUpdate(float deltaTime) {
	//Check if player is close enough to spawn
	Player* player = mGame->GetPlayer();
	if (player != nullptr && Math::Abs(player->GetPosition().x - mPosition.x) < SPAWN_DIST) {
		//Make the new goomba
		Goomba* newGoomba = new Goomba(mGame);
		newGoomba->SetPosition(mPosition);
		
		//Delete spawner
		SetState(ActorState::Destroy);
	}
}