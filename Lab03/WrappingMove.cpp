#include "WrappingMove.h"
#include "Actor.h"
#include "Game.h"

WrappingMove::WrappingMove(Actor* owner, Vector2 mDir) : MoveComponent(owner) {
	moveDirection = mDir;
}

void WrappingMove::Update(float deltaTime) {
	//Make move
	mOwner->SetPosition(mOwner->GetPosition() + moveDirection * deltaTime * mForwardSpeed);

	//Wrap
	Vector2 ownerPos = mOwner->GetPosition();
	float gameWidth = (float)mOwner->GetGame()->WIDTH;
	if (ownerPos.x < 0) {
		mOwner->SetPosition(Vector2(gameWidth, ownerPos.y));
	}
	else if (ownerPos.x > gameWidth) {
		mOwner->SetPosition(Vector2(0, ownerPos.y));
	}
}