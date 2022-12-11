#include "Effect.h"
#include "Actor.h"
#include "Math.h"
#include <string>
#include "Game.h"
#include "AnimatedSprite.h"
#include "SDL2/SDL_mixer.h"

Effect::Effect(Game* game, Vector2 pos, std::string animationName, std::string soundName) : Actor(game) {
	//Move to where specified
	mPosition = pos;

	//Set animation
	AnimatedSprite* spriteComp = new AnimatedSprite(this);
	spriteComp->LoadAnimations("Assets/Effects");
	spriteComp->SetAnimation(animationName);
	lifetime = spriteComp->GetAnimDuration(animationName);

	//Play sound
	Mix_PlayChannel(-1, game->GetSound(soundName), 0);
}

void Effect::OnUpdate(float deltaTime) {
	lifetime -= deltaTime;
	if (lifetime <= 0.0f) {
		SetState(ActorState::Destroy);
	}
}
