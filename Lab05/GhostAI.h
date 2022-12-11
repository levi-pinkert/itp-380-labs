#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>
#include <string>

class GhostAI : public Component
{
public:
	// Used to track the four different GhostAI states
	enum State
	{
		Scatter,
		Chase,
		Frightened,
		Dead
	};
	
	GhostAI(class Actor* owner);

	void Update(float deltaTime) override;
	
	// Called when the Ghost starts at the beginning
	// (or when the ghosts should respawn)
	void Start(class PathNode* startNode);
	
	// Get the current state
	State GetState() const { return mState; }
	
	// Called when the ghost should switch to the "Frightened" state
	void Frighten();
	
	// Called when the ghost should switch to the "Dead" state
	void Die();
	
	//  Helper function to draw GhostAI's current goal, prev, and next
	void DebugDrawPath(struct SDL_Renderer* render);
private:
	// Member data for pathfinding

	// TargetNode is our current goal node
	class PathNode* mTargetNode = nullptr;
	// PrevNode is the last node we intersected
	// with prior to the current position
	class PathNode* mPrevNode = nullptr;
	// NextNode is the next node we're trying to get to
	class PathNode* mNextNode = nullptr;

	// Current state of the Ghost AI
	State mState = Scatter;

	// Save the owning actor (cast to a Ghost*)
	class Ghost* mGhost;

	Vector2 moveDir;
	float stateTimer;

	PathNode* FindTargetNode();
	PathNode* FindNextNode();
	void SetNextNode(PathNode* newNextNode);
	std::vector<PathNode*> GetValidNeighbors(PathNode* node);
	void ChangeState();
	void UpdateAnimation();
	PathNode* GetClosestDefaultNode(Vector2 pos);

	const std::string DEAD_ANIMS[4] = { "deadright", "deadup", "deadleft", "deaddown" };
	const std::string ALIVE_ANIMS[4] = { "right", "up", "left", "down" };
};
