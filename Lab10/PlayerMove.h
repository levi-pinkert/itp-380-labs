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
		Falling,
		WallClimb,
		WallRun
	};
	void ChangeState(MoveState newState);
	void PhysicsUpdate(float deltaTime);
	void AddForce(const Vector3& force);
protected:
	CollSide FixCollision(class CollisionComponent* self, class CollisionComponent* block);
private:
	const float SPEED = 700.0f;
	const float MOUSE_SENSITIVITY = 20.0f;	//10 is default from lab
	const float MAX_SPEED = 400.0f;
	const float BRAKING_FACTOR = 0.9f;
	const float BRAKING_THRESH = 0.1f;
	const float WALLCLIMB_FACING_THRESH = 0.5f;
	const float WALLCLIMB_MOVEANGLE_THRESH = 0.8f;
	const float WALLCLIMB_SPEED_THRESH = 350.0f;
	const Vector3 WALLCLIMB_FORCE = Vector3(0.0f, 0.0f, 1800.0f);
	const float WALLCLIMB_LENGTH = 0.4f;
	const float NORMAL_FORCE = 700.0f;
	const Vector3 WALLRUN_FORCE = Vector3(0.0f, 0.0f, 1200.0f);
	const float WALLRUN_FACING_THRESH = 0.5f;
	const float WALLRUN_SPEED_THRESH = 350.0f;
	const float WALLRUN_VDIR_THRESH = 0.2f;
	Vector3 mVelocity;
	Vector3 mAcceleration;
	Vector3 mPendingForces;
	float mMass = 1.0f;
	Vector3 mGravity = Vector3(0.0f, 0.0f, -980.0f);
	Vector3 mJumpForce = Vector3(0.0f, 0.0f, 35000.0f);
	Player* mPlayer = nullptr;
	MoveState mCurrentState = Falling;
	bool lastJumpInput = false;
	float mWallClimbTimer = 0.0f;
	float mWallRunTimer = 0.0f;
	void UpdateOnGround(float deltaTime);
	void UpdateJump(float deltaTime);
	void UpdateFalling(float deltaTime);
	void UpdateWallClimb(float deltaTime);
	void UpdateWallRun(float deltaTime);
	void FixXYVelocity();
	bool CanClimbWall(CollSide collSide);
	bool CanWallRun(CollSide collSide);
	Vector3 CollSideNormal(CollSide collSide);
};