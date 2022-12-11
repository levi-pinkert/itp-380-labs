#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game)
	:mGame(game)
	,mState(ActorState::Active)
	,mPosition(Vector2::Zero)
	,mScale(1.0f)
	,mRotation(0.0f)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	mGame->RemoveActor(this);
	for (size_t i = 0; i < mComponents.size(); i++) {
		delete mComponents[i];
	}
	mComponents.clear();
}

void Actor::Update(float deltaTime)
{
	if (mState == ActorState::Active) {
		//Update all components
		for (size_t i = 0; i < mComponents.size(); i++) {
			mComponents[i]->Update(deltaTime);
		}
		//Run Actor's Update Code
		OnUpdate(deltaTime);
	}
}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState)
{
	if (mState == ActorState::Active) {
		//Let all components process input
		for (size_t i = 0; i < mComponents.size(); i++) {
			mComponents[i]->ProcessInput(keyState);
		}
		//Actor's Input Processing
		OnProcessInput(keyState);
	}
}

void Actor::OnProcessInput(const Uint8* keyState)
{
}

void Actor::AddComponent(Component* c)
{
	mComponents.emplace_back(c);
	std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
		return a->GetUpdateOrder() < b->GetUpdateOrder();
	});
}

Vector2 Actor::GetForward() const {
	return Vector2(Math::Cos(mRotation), -Math::Sin(mRotation));
}
