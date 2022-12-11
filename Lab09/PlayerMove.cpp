#include "PlayerMove.h"
#include "Actor.h"
#include "Math.h"
#include "SDL2/SDL.h"
#include "Game.h"
#include "Renderer.h"
#include "Random.h"
#include "Player.h"
#include "MoveComponent.h"
#include "CameraComponent.h"
#include "CollisionComponent.h"

PlayerMove::PlayerMove(Actor* actor) : MoveComponent(actor) {
	//Initialize member variables
	mPlayer = static_cast<Player*>(actor);
	ChangeState(Falling);
}

void PlayerMove::ProcessInput(const Uint8* keyState) {
	//Process forward input (W & S)
	float fwdInput = (float)(keyState[SDL_SCANCODE_W]) - (float)(keyState[SDL_SCANCODE_S]);
	AddForce(mOwner->GetForward() * fwdInput * SPEED);
	
	//Process strafe input (A & D)
	float strafeInput = (float)(keyState[SDL_SCANCODE_D]) - (float)(keyState[SDL_SCANCODE_A]);
	AddForce(mOwner->GetRight() * strafeInput * SPEED);

	//Process mouse input
	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	float yawSpeed = x / 500.0f * Math::Pi * MOUSE_SENSITIVITY;
	SetAngularSpeed(yawSpeed);
	float pitchSpeed = y / 500.0f * Math::Pi * MOUSE_SENSITIVITY;
	mOwner->GetComponent<CameraComponent>()->SetPitchSpeed(pitchSpeed);

	//Process jump input
	if (mCurrentState == OnGround && !lastJumpInput && keyState[SDL_SCANCODE_SPACE]) {
		AddForce(mJumpForce);
		ChangeState(Jump);
	}
	lastJumpInput = keyState[SDL_SCANCODE_SPACE];
}

void PlayerMove::Update(float deltaTime) {
	if (mCurrentState == OnGround) {
		UpdateOnGround(deltaTime);
	}
	else if (mCurrentState == Jump) {
		UpdateJump(deltaTime);
	}
	else if (mCurrentState == Falling) {
		UpdateFalling(deltaTime);
	}
}

void PlayerMove::UpdateOnGround(float deltaTime) {
	//Move according to physics
	PhysicsUpdate(deltaTime);

	//Check collisions, and check if player is still on ground
	std::vector<Actor*>& blocks = mOwner->GetGame()->GetBlocks();
	bool onGround = false;
	CollisionComponent* collisionComp = mOwner->GetComponent<CollisionComponent>();
	for (Actor* block : blocks) {
		CollSide collSide = FixCollision(collisionComp, block->GetComponent<CollisionComponent>());
		if (collSide == CollSide::Top) {
			onGround = true;
		}
	}
	if (!onGround) {
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateJump(float deltaTime) {
	//Move
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	//Collide
	std::vector<Actor*>& blocks = mOwner->GetGame()->GetBlocks();
	CollisionComponent* collisionComp = mOwner->GetComponent<CollisionComponent>();
	for (Actor* block : blocks) {
		CollSide collSide = FixCollision(collisionComp, block->GetComponent<CollisionComponent>());
		if (collSide == CollSide::Top) {
			mVelocity.z = 0.0f;
			ChangeState(OnGround);
		}
		else if (collSide == CollSide::Bottom) {
			mVelocity.z = 0.0f;
		}
	}

	//Check if we have begun falling
	if (mVelocity.z <= 0.0f) {
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateFalling(float deltaTime) {
	//Move
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	//Collide
	std::vector<Actor*>& blocks = mOwner->GetGame()->GetBlocks();
	CollisionComponent* collisionComp = mOwner->GetComponent<CollisionComponent>();
	for (Actor* block : blocks) {
		CollSide collSide = FixCollision(collisionComp, block->GetComponent<CollisionComponent>());
		if (collSide == CollSide::Top) {
			mVelocity.z = 0.0f;
			ChangeState(OnGround);
		}
	}
}

void PlayerMove::ChangeState(MoveState newState) {
	mCurrentState = newState;
}

CollSide PlayerMove::FixCollision(CollisionComponent* self, CollisionComponent* block) {
	Vector3 offset = Vector3::Zero;
	CollSide collSide = self->GetMinOverlap(block, offset);
	if (collSide != CollSide::None) {
		mOwner->SetPosition(mOwner->GetPosition() + offset);
	}
	return collSide;
}

void PlayerMove::PhysicsUpdate(float deltaTime) {
	//Move and accelerate
	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += mAcceleration * deltaTime;
	FixXYVelocity();
	mOwner->SetPosition(mOwner->GetPosition() + mVelocity * deltaTime);

	//Update rotation
	mOwner->SetRotation(mOwner->GetRotation() + mAngularSpeed * deltaTime);

	//Reset pending forces
	mPendingForces = Vector3::Zero;
}

void PlayerMove::AddForce(const Vector3& force) {
	mPendingForces += force;
}

void PlayerMove::FixXYVelocity() {
	Vector2 xyVelocity(mVelocity.x, mVelocity.y);

	//Cap speed
	float xySpd = xyVelocity.Length();
	if (xySpd > MAX_SPEED) {
		xyVelocity *= MAX_SPEED / xySpd;
	}

	//Brake
	if (mCurrentState == OnGround) {
		if (Math::Abs(mAcceleration.x) < BRAKING_THRESH || (mAcceleration.x > 0.0f != mVelocity.x > 0.0f)) {
			xyVelocity.x *= BRAKING_FACTOR;
		}
		if (Math::Abs(mAcceleration.y) < BRAKING_THRESH || (mAcceleration.y > 0.0f != mVelocity.y > 0.0f)) {
			xyVelocity.y *= BRAKING_FACTOR;
		}
	}

	//Apply result
	mVelocity.x = xyVelocity.x;
	mVelocity.y = xyVelocity.y;
}