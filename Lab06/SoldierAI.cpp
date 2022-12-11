#include "SoldierAI.h"
#include "Actor.h"
#include "Game.h"
#include "PathFinder.h"
#include "PathNode.h"
#include "AnimatedSprite.h"
#include <algorithm>
#include "Math.h"

SoldierAI::SoldierAI(Actor* owner)
:SpriteComponent(owner)
{
	SetIsVisible(false); // Uncomment to hide debug paths
	stunTimer = 0.0f;
}

void SoldierAI::Setup(PathNode* start, PathNode* end)
{
	//Find the path to take
	mPatrolStart = start;
	mPatrolEnd = end;
	mOwner->GetGame()->GetPathFinder()->CalculatePath(mPatrolStart, mPatrolEnd, mPath);

	//Initialize other variables appropriately
	mPrev = mPatrolStart;
	StartNextMove();
}

void SoldierAI::Update(float deltaTime)
{
	//Update stun
	stunTimer = Math::Max(stunTimer -= deltaTime, 0.0f);

	//Move towards next node
	if (stunTimer == 0.0f) {
		mOwner->SetPosition(mOwner->GetPosition() + moveDir * SOLDIER_SPEED * deltaTime);
	}

	//Check if we've reached the next node
	if (Vector2::Distance(mOwner->GetPosition(), mNext->GetPosition()) < 1.0f) {
		mOwner->SetPosition(mNext->GetPosition());
		if (!mPath.empty()) {
			mPrev = mNext;
			StartNextMove();
		}
		else {
			Setup(mPatrolEnd, mPatrolStart);
		}
	}

	//Update the animations
	UpdateAnimation();
}

void SoldierAI::StartNextMove() {
	mNext = mPath.back();
	mPath.pop_back();
	moveDir = mNext->GetPosition() - mPrev->GetPosition();
	moveDir.Normalize();
}

void SoldierAI::UpdateAnimation() {
	//Set the sprite properly
	AnimatedSprite* spriteComp = mOwner->GetComponent<AnimatedSprite>();
	if (moveDir.x > 0.0f) {
		spriteComp->SetAnimation("WalkRight");
	}else if (moveDir.y < 0.0f) {
		spriteComp->SetAnimation("WalkUp");
	}else if (moveDir.x < 0.0f) {
		spriteComp->SetAnimation("WalkLeft");
	}else if (moveDir.y > 0.0f) {
		spriteComp->SetAnimation("WalkDown");
	}

	//Pause or unpause based on stun status
	spriteComp->SetIsPaused(stunTimer > 0.0f);
}

void SoldierAI::Stun() {
	stunTimer = STUN_DURATION;
}

// This helper is to just debug draw the soldier's path to visualize it
// (only called if this component is set to visible)
void SoldierAI::Draw(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();
	
	// Draw from current position to next
	SDL_RenderDrawLine(renderer,
					   static_cast<int>(mOwner->GetPosition().x - cameraPos.x),
					   static_cast<int>(mOwner->GetPosition().y - cameraPos.y),
					   static_cast<int>(mNext->GetPosition().x - cameraPos.x),
					   static_cast<int>(mNext->GetPosition().y - cameraPos.y));

	// Draw from next to first node on path
	if (!mPath.empty())
	{
		// Draw from current position to next
		SDL_RenderDrawLine(renderer,
						   static_cast<int>(mNext->GetPosition().x - cameraPos.x),
						   static_cast<int>(mNext->GetPosition().y - cameraPos.y),
						   static_cast<int>(mPath.back()->GetPosition().x - cameraPos.x),
						   static_cast<int>(mPath.back()->GetPosition().y - cameraPos.y));
	}
	
	// Draw each node on the path
	if (mPath.size() > 1)
	{
		for (size_t i = 0; i < mPath.size() - 1; i++)
		{
			SDL_RenderDrawLine(renderer,
							   static_cast<int>(mPath[i]->GetPosition().x - cameraPos.x),
							   static_cast<int>(mPath[i]->GetPosition().y - cameraPos.y),
							   static_cast<int>(mPath[i + 1]->GetPosition().x - cameraPos.x),
							   static_cast<int>(mPath[i + 1]->GetPosition().y - cameraPos.y));
		}
	}
}
