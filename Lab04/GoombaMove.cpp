#include "GoombaMove.h"
#include <vector>
#include "Block.h"
#include "CollisionComponent.h"
#include "Math.h"
#include "Game.h"
#include "SDL2/SDL.h"
#include "Goomba.h"

GoombaMove::GoombaMove(Actor* owner) : MoveComponent(owner) {
	//Initialize member variables
	moveDir = -1.0f;
	ySpeed = 0.0f;
	canMove = true;
}

void GoombaMove::Update(float deltaTime) {
	//Move vertically
	mOwner->SetPosition(mOwner->GetPosition() + Vector2(0.0f, ySpeed * deltaTime));

	//First pass of collisions for floors
	const std::vector<Block*>& blocks = mOwner->GetGame()->GetBlocks();
	CollisionComponent* goombaCollider = mOwner->GetComponent<CollisionComponent>();
	for (Block* block : blocks) {
		Vector2 offset = Vector2::Zero;
		CollSide collSide = goombaCollider->GetMinOverlap(block->GetCollisionComponent(), offset);

		//If the goomba lands on the ground, kill their y velocity
		if (collSide == CollSide::Top && ySpeed > 0.0f) {
			ySpeed = 0.0f;
			mOwner->SetPosition(mOwner->GetPosition() + offset);
		}
	}

	//Move horizontally
	if (canMove) {
		mOwner->SetPosition(mOwner->GetPosition() + Vector2(moveDir * MOVE_SPEED * deltaTime, 0.0f));
	}

	//Second pass of collision for horizontal collisions with walls
	for (Block* block : blocks) {
		Vector2 offset = Vector2::Zero;
		CollSide collSide = goombaCollider->GetMinOverlap(block->GetCollisionComponent(), offset);

		if ((moveDir < 0.0f && collSide == CollSide::Right) || (moveDir > 0.0f && collSide == CollSide::Left)) {
			//The goomba moves left into the right side of a wall
			moveDir *= -1.0f;
			mOwner->SetPosition(mOwner->GetPosition() + offset);
		}
	}

	//Third pass of collision for horizontal collisions with goombas
	const std::vector<Goomba*>& goombas = mOwner->GetGame()->GetGoombas();
	for (Goomba* other : goombas) {
		if (other != mOwner) {
			Vector2 offset = Vector2::Zero;
			CollSide collSide = goombaCollider->GetMinOverlap(other->GetCollisionComponent(), offset);

			if ((moveDir < 0.0f && collSide == CollSide::Right) || (moveDir > 0.0f && collSide == CollSide::Left)) {
				//The goomba moves left into the right side of another goomba
				moveDir *= -1.0f;
				mOwner->SetPosition(mOwner->GetPosition() + offset);
			}
		}
	}

	//Die if below screen
	if (mOwner->GetPosition().y >= MAX_Y) {
		mOwner->SetState(ActorState::Destroy);
	}

	//Accelerate according to gravity
	ySpeed += GRAVITY * deltaTime;
}

void GoombaMove::SetCanMove(bool newCanMove) {
	canMove = newCanMove;
}