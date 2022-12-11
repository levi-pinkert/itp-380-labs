#include "Actor.h"
class WrappingMove;
class SpriteComponent;
class CollisionComponent;

class Vehicle : public Actor {
public:
	Vehicle(Game*, char type, Vector2 moveDir);
	virtual ~Vehicle() override;
	virtual void OnUpdate(float deltaTime) override;
	CollisionComponent* GetCollisionComponent();
private:
	const float MOVE_SPEED = 100.0f;
	const float SLOW_ANGLE = Math::Pi / 6.0f;
	const float SLOW_FACTOR = 0.5f;
	WrappingMove* moveComp;
	SpriteComponent* spriteComp;
	CollisionComponent* collisionComp;
};