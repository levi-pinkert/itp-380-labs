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
#include "Checkpoint.h"

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
	//Update according to state
	if (mCurrentState == OnGround) {
		UpdateOnGround(deltaTime);
	}
	else if (mCurrentState == Jump) {
		UpdateJump(deltaTime);
	}
	else if (mCurrentState == Falling) {
		UpdateFalling(deltaTime);
	}
	else if (mCurrentState == WallClimb) {
		UpdateWallClimb(deltaTime);
	}
	else if (mCurrentState == WallRun) {
		UpdateWallRun(deltaTime);
	}

	//If fell out of the world, respawn
	if (mOwner->GetPosition().z < MIN_Z) {
		Respawn();
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
		//Try to climb this wall
		if ((collSide == CollSide::Left || collSide == CollSide::Right || collSide == CollSide::Front || collSide == CollSide::Back) && CanClimbWall(collSide)) {
			mWallClimbTimer = 0.0f;
			ChangeState(WallClimb);
			return;
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

		//Try to climb this wall
		if (collSide == CollSide::Left || collSide == CollSide::Right || collSide == CollSide::Front || collSide == CollSide::Back) {
			if (CanClimbWall(collSide)) {
				mWallClimbTimer = 0.0f;
				ChangeState(WallClimb);
				return;
			}
			else if (CanWallRun(collSide)) {
				mWallRunTimer = 0.0f;
				ChangeState(WallRun);
				mOwner->GetComponent<CameraComponent>()->SetWallRunNormal(CollSideNormal(collSide));
				return;
			}
			
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

void PlayerMove::UpdateWallClimb(float deltaTime) {
	mWallClimbTimer += deltaTime;

	//Move
	AddForce(mGravity);
	if (mWallClimbTimer < WALLCLIMB_LENGTH) {
		AddForce(WALLCLIMB_FORCE);
	}
	PhysicsUpdate(deltaTime);

	//Collide
	std::vector<Actor*>& blocks = mOwner->GetGame()->GetBlocks();
	bool onWall = false;
	CollisionComponent* collisionComp = mOwner->GetComponent<CollisionComponent>();
	for (Actor* block : blocks) {
		CollSide collSide = FixCollision(collisionComp, block->GetComponent<CollisionComponent>());
		if (collSide == CollSide::Left || collSide == CollSide::Right || collSide == CollSide::Front || collSide == CollSide::Back) {
			onWall = true;
		}
	}

	//End wallclimb
	if (mVelocity.z <= 0.0f || !onWall) {
		mVelocity.z = 0.0f;
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateWallRun(float deltaTime) {
	mWallRunTimer += deltaTime;

	//Move
	AddForce(mGravity);
	if (mWallRunTimer < WALLCLIMB_LENGTH) {
		AddForce(WALLRUN_FORCE);
	}
	PhysicsUpdate(deltaTime);

	//Collide
	std::vector<Actor*>& blocks = mOwner->GetGame()->GetBlocks();
	CollisionComponent* collisionComp = mOwner->GetComponent<CollisionComponent>();
	for (Actor* block : blocks) {
		FixCollision(collisionComp, block->GetComponent<CollisionComponent>());
	}

	//End wallrun
	if (mVelocity.z <= 0.0f) {
		mOwner->GetComponent<CameraComponent>()->SetWallRunNormal(Vector3::Zero);
		ChangeState(Falling);
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
	if (collSide == CollSide::Left || collSide == CollSide::Right || collSide == CollSide::Front || collSide == CollSide::Back) {
		AddForce(CollSideNormal(collSide) * NORMAL_FORCE);
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
	if (mCurrentState == OnGround || mCurrentState == WallClimb) {
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

Vector3 PlayerMove::CollSideNormal(CollSide collSide) {
	if (collSide == CollSide::Front) {
		return Vector3::UnitX;
	}
	else if (collSide == CollSide::Back) {
		return Vector3::NegUnitX;
	}
	else if (collSide == CollSide::Right) {
		return Vector3::UnitY;
	}
	else if (collSide == CollSide::Left) {
		return Vector3::NegUnitY;
	}
	else if (collSide == CollSide::Top) {
		return Vector3::UnitZ;
	}
	else if (collSide == CollSide::Bottom) {
		return Vector3::NegUnitZ;
	}
	return Vector3::Zero;
}

bool PlayerMove::CanClimbWall(CollSide collSide) {
	//Find the normal of the side
	Vector3 normal = CollSideNormal(collSide);
	
	//Make sure it's a vertical wall
	if (normal.z != 0.0f) { return false; }

	//Make sure the player is somewhat facing the wall
	Vector3 forward = mOwner->GetForward();
	if (Vector3::Dot(forward, normal) > -WALLCLIMB_FACING_THRESH) { return false; }

	//Make sure the player is moving fast enough
	Vector3 xyVelocity(mVelocity);
	xyVelocity.z = 0.0f;
	float xySpd = xyVelocity.Length();
	if (xySpd <= WALLCLIMB_SPEED_THRESH) { return false; }

	//Make sure the player is moving towards the wall
	xyVelocity *= 1.0f / xySpd;
	if (Vector3::Dot(xyVelocity, normal) > -WALLCLIMB_MOVEANGLE_THRESH) { return false; }

	//The player passed all the checks, so they can wallclimb!
	return true;
}

bool PlayerMove::CanWallRun(CollSide collSide) {
	//Find the normal of the side
	Vector3 normal = CollSideNormal(collSide);

	//Make sure it's a vertical wall
	if (normal.z != 0.0f) { return false; }

	//Make sure the player is facing somewhat perpendicular to the wall
	Vector3 forward = mOwner->GetForward();
	if (Math::Abs(Vector3::Dot(forward, normal)) > WALLRUN_FACING_THRESH) { return false; }

	//Make sure the player is moving fast enough
	Vector3 xyVelocity(mVelocity);
	xyVelocity.z = 0.0f;
	float xySpd = xyVelocity.Length();
	if (xySpd <= WALLRUN_SPEED_THRESH) { return false; }

	//Make sure the player is moving the same direction as they're looking
	xyVelocity *= 1.0f / xySpd;
	if (Vector3::Dot(xyVelocity, forward) < WALLRUN_VDIR_THRESH) { return false; }

	//The player passed all the checks, so they can wallclimb!
	return true;
}

void PlayerMove::Respawn() {
	mOwner->SetPosition(mOwner->GetGame()->GetPlayer()->GetRespawnPos());
	mOwner->SetRotation(0.0f);
	mVelocity = mPendingForces = Vector3::Zero;
	ChangeState(Falling);
}