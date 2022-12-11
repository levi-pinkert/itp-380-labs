#include "MoveComponent.h"
#include "Math.h"
#include <vector>
class Actor;
class Player;

class PlayerMove : public MoveComponent {
public:
	PlayerMove(Actor* actor);
	virtual void Update(float deltaTime);
	virtual void ProcessInput(const Uint8* keyState);
private:
	const float MOVE_SPEED = 400.0f;
	const float STRAFE_SPEED = 300.0f;
	const float MAX_Y = 180.0f;
	const float MAX_Z = 225.0f;
	const float CAM_H_DIST = 300.0f;
	const float CAM_TARGET_DIST = 20.0f;
	const float WALL_SPAWN_DIST = 4000.0f;
	const float WALL_SIZE = 500.0f;
	const float LEVEL_SEPARATION = 1000.0f;
	const float ACCEL_TIME = 10.0f;
	const float ACCEL_AMOUNT = 0.15f;
	const float ROLL_LENGTH = 0.5f;
	const float ROLL_SPEED = 8.0f * Math::Pi;
	const std::vector<size_t> SIDE_PATTERN = { 0, 1, 2, 0 };
	Vector3 mVelocity;
	int wallsSpawned;
	int levelsLoaded;
	bool lastShootInput;
	Player* player;
	float spdMult;
	float accelTimer;
	float rollTimer;
	bool lastRollInput;
	void CreateWallSet();
	void CheckBlockCollision();
	void EulerIntegrate(float deltaTime);
	void UpdateCameraPos();
};