#include "MoveComponent.h"
#include "Math.h"
class Actor;

class PlayerMove : public MoveComponent {
public:
	PlayerMove(Actor* owner);
	virtual void ProcessInput(const Uint8* keyState);
	virtual void Update(float deltaTime);
private:
	const float MOVE_SPEED = 300.0f;
	const float GRAVITY = 2000.0f;
	const float MAX_Y = 448.0f;
	const float JUMP_SPEED = -700.0f;
	const float WIN_X = 6368.0f;
	float mYSpeed;
	bool mSpacePressed;
	bool mInAir;
	bool ownerDead;
	void UpdateAnimation();
	void KillOwner();
};