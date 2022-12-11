#include "MoveComponent.h"
#include "Math.h"
class Actor;

class GoombaMove : public MoveComponent {
public:
	GoombaMove(Actor* owner);
	virtual void Update(float deltaTime);
	void SetCanMove(bool newCanMove);
private:
	const float MOVE_SPEED = 100.0f;
	const float GRAVITY = 2000.0f;
	const float MAX_Y = 448.0f;
	float moveDir;
	float ySpeed;
	bool canMove;
};