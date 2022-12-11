#include "DeadFrog.h"
#include "SpriteComponent.h"
#include "Game.h"

DeadFrog::DeadFrog(Game* game) : Actor(game) {
	//Init despawn timer
	timer = DESPAWN_TIME;

	//Set texture
	spriteComp = new SpriteComponent(this);
	spriteComp->SetTexture(game->GetTexture("Assets/Dead.png"));
}

void DeadFrog::OnUpdate(float deltaTime) {
	//Decrement despawn timer and check if should be destroyed
	timer -= deltaTime;
	if (timer <= 0.0f) {
		SetState(ActorState::Destroy);
	}
}