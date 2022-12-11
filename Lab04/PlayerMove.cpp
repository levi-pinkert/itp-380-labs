#include "PlayerMove.h"
#include "Actor.h"
#include "SDL2/SDL.h"
#include "Math.h"
#include <algorithm>
#include "Game.h"
#include "Block.h"
#include <vector>
#include "CollisionComponent.h"
#include "Goomba.h"
#include "SpriteComponent.h"
#include "AnimatedSprite.h"
#include "SDL2/SDL_mixer.h"

PlayerMove::PlayerMove(Actor* actor) : MoveComponent(actor) {
	//Initialize member variables
	mYSpeed = 0.0f;
	mSpacePressed = false;
	mInAir = false;
	ownerDead = false;
}

void PlayerMove::ProcessInput(const Uint8* keyState) {
	//Set forward speed based on left and right key input
	int moveInput = (int)(keyState[SDL_SCANCODE_RIGHT]) - (int)(keyState[SDL_SCANCODE_LEFT]);
	SetForwardSpeed(moveInput * MOVE_SPEED);

	//Jump if space leading edge detected
	if (!mSpacePressed && !mInAir && keyState[SDL_SCANCODE_SPACE]) {
		mYSpeed = JUMP_SPEED;
		mInAir = true;
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Jump.wav"), 0);
	}
	mSpacePressed = keyState[SDL_SCANCODE_SPACE];
}

void PlayerMove::Update(float deltaTime) {
	//Move according to y velocity
	Vector2 ownerPos = mOwner->GetPosition();
	mOwner->SetPosition(Vector2(ownerPos.x, ownerPos.y + mYSpeed * deltaTime));

	//Check for collisions with blocks vertically
	const std::vector<Block*>& blocks = mOwner->GetGame()->GetBlocks();
	bool hasCollided = false;
	CollisionComponent* playerCollider = mOwner->GetComponent<CollisionComponent>();
	for (Block* block : blocks) {
		//Check the collision between player and block
		Vector2 offset = Vector2::Zero;
		CollSide collSide = playerCollider->GetMinOverlap(block->GetCollisionComponent(), offset);

		//If the player lands on the ground, kill their y velocity
		if (collSide == CollSide::Top && mYSpeed > 0.0f) {
			mYSpeed = 0.0f;
			mInAir = false;
		}

		//If the player bonks their head, kill their y velocity
		if (collSide == CollSide::Bottom && mYSpeed < 0.0f) {
			mYSpeed = 0.0f;
			Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Bump.wav"), 0);
		}

		//If the player is in a block, move them out
		if (collSide == CollSide::Top || collSide == CollSide::Bottom) {
			mOwner->SetPosition(mOwner->GetPosition() + offset);
			hasCollided = true;
		}
	}

	//Move according to forward speed
	ownerPos = mOwner->GetPosition();
	Vector2& cameraPos = mOwner->GetGame()->GetCameraPos();
	mOwner->SetPosition(Vector2(std::max(ownerPos.x + GetForwardSpeed() * deltaTime, cameraPos.x), ownerPos.y));

	//Check for collisions with blocks horizontally
	for (Block* block : blocks) {
		//Check the collision between player and block
		Vector2 offset = Vector2::Zero;
		CollSide collSide = playerCollider->GetMinOverlap(block->GetCollisionComponent(), offset);

		//If the player is in a block, move them out
		if (collSide == CollSide::Left || collSide == CollSide::Right) {
			mOwner->SetPosition(mOwner->GetPosition() + offset);
			hasCollided = true;
		}
	}

	//If we're not colliding with a block, we must be in the air
	if (!hasCollided) {
		mInAir = true;
	}

	//Check for collision with goombas
	const std::vector<Goomba*>& goombas = mOwner->GetGame()->GetGoombas();
	for (Goomba* goomba : goombas) {
		if (!goomba->GetStomped()) {
			Vector2 offset = Vector2::Zero;
			CollSide collSide = playerCollider->GetMinOverlap(goomba->GetCollisionComponent(), offset);

			if (collSide != CollSide::None) {
				if (collSide == CollSide::Top || (collSide != CollSide::Bottom && mInAir)) {
					//Bounce on the goomba
					goomba->Stomp();
					mYSpeed = JUMP_SPEED / 2.0f;
					mInAir = true;
					Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Stomp.wav"), 0);
				}
				else {
					//Die to the goomba
					KillOwner();
				}
			}
		}
	}

	//Check if fell off world
	if (mOwner->GetPosition().y >= MAX_Y) {
		KillOwner();
	}

	//Check if won the level
	if (mOwner->GetPosition().x >= WIN_X) {
		mOwner->SetState(ActorState::Paused);
		mOwner->GetGame()->StopMusic();
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/StageClear.wav"), 0);
	}

	//Apply gravity to velocity
	mYSpeed += deltaTime * GRAVITY;

	//Move the camera
	cameraPos.x = std::max(mOwner->GetPosition().x - 300.0f, cameraPos.x);

	//Update the player's animation
	UpdateAnimation();
}

void PlayerMove::KillOwner() {
	mOwner->GetComponent<SpriteComponent>()->SetTexture(mOwner->GetGame()->GetTexture("Assets/Mario/Dead.png"));
	mOwner->SetState(ActorState::Paused);
	mOwner->GetGame()->StopMusic();
	Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Dead.wav"), 0);
	ownerDead = true;
}

void PlayerMove::UpdateAnimation() {
	//Get the animation component
	AnimatedSprite* animComp = mOwner->GetComponent<AnimatedSprite>();

	//Pick the animation
	float xSpd = GetForwardSpeed();
	std::string currentAnim = animComp->GetAnimName();
	if (ownerDead) {
		//If actor is paused, assume it's dead, since that's the only time it'll be paused
		animComp->SetAnimation("dead");
	}
	else if (mInAir) {
		//In air
		if (xSpd > 0.0f) {
			//Jumping right
			animComp->SetAnimation("jumpRight");
		}
		else if (xSpd < 0.0f) {
			//Jumping left
			animComp->SetAnimation("jumpLeft");
		}
		else {
			//It depends what we were doing last frame
			if (currentAnim == "runRight" || currentAnim == "jumpRight" || currentAnim == "idle") {
				animComp->SetAnimation("jumpRight");
			}
			else {
				animComp->SetAnimation("jumpLeft");
			}
		}
	}
	else {
		//On ground
		if (xSpd > 0.0f) {
			//Running right
			animComp->SetAnimation("runRight");
		}
		else if (xSpd < 0.0f) {
			//Jumping left
			animComp->SetAnimation("runLeft");
		}
		else {
			//Idling
			animComp->SetAnimation("idle");
		}
	}
}