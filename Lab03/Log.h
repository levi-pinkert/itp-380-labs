#include "Actor.h"
class WrappingMove;
class SpriteComponent;
class CollisionComponent;

class Log : public Actor {
public:
	Log(Game*, char type, Vector2 moveDir);
	virtual ~Log();
	CollisionComponent* GetCollisionComponent();
	WrappingMove* GetWrappingMoveComponent();
private:
	const float MOVE_SPEED = 75.0f;
	WrappingMove* moveComp;
	SpriteComponent* spriteComp;
	CollisionComponent* collisionComp;
};