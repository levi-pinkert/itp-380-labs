#include "GhostAI.h"
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "PathNode.h"
#include "AnimatedSprite.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include "Ghost.h"
#include "PacMan.h"
#include "Random.h"
#include "CollisionComponent.h"
#include <vector>

GhostAI::GhostAI(class Actor* owner)
:Component(owner, 50)
{
	mGhost = static_cast<Ghost*>(owner);
	stateTimer = 0.0f;
}

void GhostAI::Update(float deltaTime)
{
	//Update state timer
	stateTimer += deltaTime;

	//Check if the ghost arrived at the next node and should find new node
	CollisionComponent* nextNodeCollision = mNextNode->GetComponent<CollisionComponent>();
	CollisionComponent* ghostCollision = mOwner->GetComponent<CollisionComponent>();
	if (ghostCollision->Intersect(nextNodeCollision)) {
		//Move the rest of the way to the node
		mOwner->SetPosition(mNextNode->GetPosition());

		//Check for state change
		ChangeState();

		//Find new target node
		mTargetNode = FindTargetNode();

		//Navigate towards that target node
		SetNextNode(FindNextNode());
	}

	//Move towards next node
	float moveSpd = 0.0f;
	switch (GetState()) {
	case(State::Scatter):
	case(State::Chase):
		moveSpd = 90.0f;
		break;
	case(State::Frightened):
		moveSpd = 65.0f;
		break;
	case(State::Dead):
		moveSpd = 125.0f;
		break;
	}
	Vector2 moveOffset = moveDir * moveSpd * deltaTime;
	mOwner->SetPosition(mOwner->GetPosition() + moveOffset);
}


void GhostAI::UpdateAnimation() {
	AnimatedSprite* animComp = mOwner->GetComponent<AnimatedSprite>();
	if (mState == Frightened) {
		animComp->SetAnimation(stateTimer < 5.0f ? "scared0" : "scared1");
	}
	else {
		//Find animation set
		const std::string* anims = (mState == Dead) ? DEAD_ANIMS : ALIVE_ANIMS;

		//Set animation based on move direction
		if (moveDir.x > 0) {
			animComp->SetAnimation(anims[0]);
		}
		else if (moveDir.y < 0) {
			animComp->SetAnimation(anims[1]);
		}
		else if (moveDir.x < 0) {
			animComp->SetAnimation(anims[2]);
		}
		else if (moveDir.y > 0) {
			animComp->SetAnimation(anims[3]);
		}
	}
}

PathNode* GhostAI::FindTargetNode()
{
	switch (GetState()) {
	case(State::Scatter):
		//Target the node saved in the ghost's member variable
		return mGhost->GetScatterNode();
	case(State::Frightened):
	{
		//Target a random (valid) adjacent node
		std::vector<PathNode*> validAdjNodes = GetValidNeighbors(mNextNode);
		if (validAdjNodes.size() == 0) { return nullptr; }
		int validNodesLen = static_cast<int>(validAdjNodes.size());
		return validAdjNodes[static_cast<size_t>(Random::GetIntRange(0, validNodesLen - 1))];
	}
	case(State::Dead):
		//Target the ghost pen
		return mGhost->GetGame()->mGhostPen;
	case(State::Chase):
		//Target the chase node, dependent on which ghost we are
		PacMan* player = mGhost->GetGame()->mPlayer;
		switch (mGhost->GetType()) {
		case(Ghost::Clyde):
			//If close to player, target scatter node
			if (Vector2::Distance(player->GetPosition(), mGhost->GetPosition()) <= 150.0f) {
				return mGhost->GetScatterNode();
			}
			//if this doesn't trigger, the switch statement will continue on to blinky's code
		case(Ghost::Blinky):
		{
			//Target pacman's previous node
			PathNode* prevNode = player->GetPrevNode();
			if (prevNode->GetType() == PathNode::Tunnel) {
				return GetClosestDefaultNode(player->GetPosition());
			}
			return prevNode;
		}
		case(Ghost::Pinky):
			//Target the node closes to 80 px in front of player
			return GetClosestDefaultNode(player->GetPointInFrontOf(80.0f));
		case(Ghost::Inky):
		{
			//Target the node on the other side of the player (roughly...)
			Vector2 ghostPos = mGhost->GetPosition();
			Vector2 v = player->GetPointInFrontOf(40.0f) - ghostPos;
			return GetClosestDefaultNode(ghostPos + 2 * v);
		}
		}
	}
	//If we didn't find a target, just return null
	return nullptr;
}

PathNode* GhostAI::GetClosestDefaultNode(Vector2 pos) {
	std::vector<PathNode*> pathNodes = mGhost->GetGame()->mPathNodes;
	float minDist = std::numeric_limits<float>::max();
	PathNode* bestNode = nullptr;
	for (PathNode* node : pathNodes) {
		float dist = Vector2::Distance(node->GetPosition(), pos);
		if (node->GetType() == PathNode::Default && dist < minDist) {
			minDist = dist;
			bestNode = node;
		}
	}
	return bestNode;
}

std::vector<PathNode*> GhostAI::GetValidNeighbors(PathNode* node) {
	//Sort the nodes into 3 categories
	const std::vector<PathNode*>& adjNodes = node->mAdjacent;
	std::vector<PathNode*> validAdjNodes, ghostAdjNodes, otherAdjNodes;
	for (PathNode* candidateNode : adjNodes) {
		PathNode::Type nodeType = candidateNode->GetType();
		if (nodeType == PathNode::Tunnel || candidateNode == mPrevNode) {
			otherAdjNodes.push_back(candidateNode);
		}
		else if (mState != Dead && nodeType == PathNode::Ghost) {
			ghostAdjNodes.push_back(candidateNode);
		}
		else {
			validAdjNodes.push_back(candidateNode);
		}
	}

	//Return the best category that isn't empty
	if (!validAdjNodes.empty()) { return validAdjNodes; }
	if (!ghostAdjNodes.empty()) { return ghostAdjNodes; }
	return otherAdjNodes;
}

PathNode* GhostAI::FindNextNode()
{
	//Find all adjacent nodes that the ghost could move to
	std::vector<PathNode*> validAdjNodes = GetValidNeighbors(mNextNode);

	//Find the closest of the valid adjacent nodes
	PathNode* nextNode = nullptr;
	float minDist = std::numeric_limits<float>::max();
	for (PathNode* node : validAdjNodes) {
		float distToTarget = Vector2::Distance(node->GetPosition(), mTargetNode->GetPosition());
		if (distToTarget < minDist) {
			nextNode = node;
			minDist = distToTarget;
		}
	}
	return nextNode;
}

void GhostAI::SetNextNode(PathNode* newNextNode)
{
	//Check if we're trying to navigate to nullptr. This shouldn't happen, but might in development
	if (newNextNode == nullptr) {
		SDL_Log("Ghost attempted to navigate to null node");
		return;
	}

	//Save current next node as previous node, then update next node
	mPrevNode = mNextNode;
	mNextNode = newNextNode;

	//Update move direction to go towards the new node
	moveDir = (mNextNode->GetPosition() - mOwner->GetPosition()) * (1.0f / Vector2::Distance(mNextNode->GetPosition(), mOwner->GetPosition()));

	//Update the animation
	UpdateAnimation();
}


void GhostAI::Frighten()
{
	//Don't do anything if dead
	if (GetState() == Dead) { return; }

	//Restart timer
	stateTimer = 0.0f;

	//If wasn't frightened before, set state and turn around
	if (mState != Frightened) {
		mState = Frightened;
		SetNextNode(mPrevNode);
	}
	UpdateAnimation();
}

void GhostAI::Start(PathNode* startNode)
{
	//Initialize position and member variables
	mOwner->SetPosition(startNode->GetPosition());
	mState = State::Scatter;
	mPrevNode = nullptr;
	mNextNode = startNode;
	stateTimer = 0.0f;
}

void GhostAI::Die()
{
	stateTimer = 0.0f;
	mState = Dead;
	UpdateAnimation();
}

void GhostAI::DebugDrawPath(SDL_Renderer* render)
{
	// Draw a rectangle at the target node
	if (mTargetNode != nullptr)
	{
		const int SIZE = 16;
		SDL_Rect r;
		r.x = static_cast<int>(mTargetNode->GetPosition().x) - SIZE / 2;
		r.y = static_cast<int>(mTargetNode->GetPosition().y) - SIZE / 2;
		r.w = SIZE;
		r.h = SIZE;
		SDL_RenderDrawRect(render, &r);
	}
	
	// Line from ghost to next node
	if (mNextNode != nullptr)
	{
		SDL_RenderDrawLine(render,
			static_cast<int>(mOwner->GetPosition().x),
			static_cast<int>(mOwner->GetPosition().y),
			static_cast<int>(mNextNode->GetPosition().x),
			static_cast<int>(mNextNode->GetPosition().y));
	}
}

void GhostAI::ChangeState() {
	//Check for frightened state expiring
	State startState = mState;
	if (mState == Frightened && stateTimer >= 7.0f) {
		mState = Scatter;
	}
	else if (mState == Dead && mPrevNode == mTargetNode) {
		mState = Scatter;
	}
	else if (mState == Scatter && stateTimer >= 5.0f) {
		mState = Chase;
	}
	else if (mState == Chase && stateTimer >= 20.0f) {
		mState = Scatter;
	}

	//Change stateTimer if necessary
	if (startState != mState) {
		stateTimer = 0.0f;
	}
}
