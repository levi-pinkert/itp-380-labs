#include "MoveComponent.h"
#include "Math.h"
class Actor;

class WrappingMove : public MoveComponent {
public:
	WrappingMove(Actor* owner, Vector2 mDir);
	virtual void Update(float deltaTime) override;
	Vector2 GetMoveDirection() {
		return moveDirection;
	}
private:
	Vector2 moveDirection;
};