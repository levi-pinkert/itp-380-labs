#include "Log.h"
#include "WrappingMove.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Math.h"
#include "CollisionComponent.h"

Log::Log(Game* game, char type, Vector2 moveDir) : Actor(game) {
	//Make move, sprite, and collision components
	moveComp = new WrappingMove(this, moveDir);
	moveComp->SetForwardSpeed(MOVE_SPEED);
	spriteComp = new SpriteComponent(this);
	collisionComp = new CollisionComponent(this);
	
	//Set values based on log type
	switch (type) {
	case 'X':
		spriteComp->SetTexture(game->GetTexture("Assets/LogX.png"));
		collisionComp->SetSize(192.0f, 48.0f);
		break;
	case 'Y':
		spriteComp->SetTexture(game->GetTexture("Assets/LogY.png"));
		collisionComp->SetSize(256.0f, 48.0f);
		break;
	case 'Z':
		spriteComp->SetTexture(game->GetTexture("Assets/LogZ.png"));
		collisionComp->SetSize(384.0f, 48.0f);
		break;
	}

	//Add self to game's log list
	mGame->AddLog(this);
}

Log::~Log() {
	//Remove self from game's log list
	mGame->RemoveLog(this);
}

CollisionComponent* Log::GetCollisionComponent()
{
	return collisionComp;
}

WrappingMove* Log::GetWrappingMoveComponent()
{
	return moveComp;
}
