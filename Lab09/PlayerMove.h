#include "Math.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
class Actor;
class Player;

class PlayerMove : public MoveComponent {
public:
	PlayerMove(Actor* actor);
	virtual void Update(float deltaTime) override;
	virtual void ProcessInput(const Uint8* keyState) override;
	enum MoveState {
		OnGround,
		Jump,
		Falling
	};
	void ChangeState(MoveState newState);
protected:
	CollSide FixCollision(class CollisionComponent* self, class CollisionComponent* block);
	void PhysicsUpdate(float deltaTime);
	void AddForce(const Vector3& force);
	void FixXYVelocity();
	void UpdateOnGround(float deltaTime);
	void UpdateJump(float deltaTime);
	void UpdateFalling(float deltaTime);
private:
	const float SPEED = 700.0f;
	const float MOUSE_SENSITIVITY = 20.0f;	//10 is default from lab
	const float MAX_SPEED = 400.0f;
	const float BRAKING_FACTOR = 0.9f;
	const float BRAKING_THRESH = 0.1f;
	Vector3 mVelocity;
	Vector3 mAcceleration;
	Vector3 mPendingForces;
	float mMass = 1.0f;
	Vector3 mGravity = Vector3(0.0f, 0.0f, -980.0f);
	Vector3 mJumpForce = Vector3(0.0f, 0.0f, 35000.0f);
	Player* mPlayer = nullptr;
	MoveState mCurrentState = Falling;
	bool lastJumpInput = false;
};