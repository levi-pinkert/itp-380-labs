#include "MoveComponent.h"
#include "Math.h"
#include <string>
class Actor;
class Sword;

class PlayerMove : public MoveComponent {
public:
	PlayerMove(Actor* owner);
	virtual void ProcessInput(const Uint8* keyState);
	virtual void Update(float deltaTime);
private:
	enum MoveDir {
		Right = 0,
		Up = 1,
		Left = 2,
		Down = 3,
		None = 4,
	};
	const float MOVE_SPEED = 150.0f;
	const float ATTACK_LEN = 0.25f;
	const float SWORD_SIZE_SHORT = 20.0f;
	const float SWORD_SIZE_LONG = 28.0f;
	const float SWORD_DIST_VERTICAL = 40.0f;
	const float SWORD_DIST_HORIZONTAL = 32.0f;
	const float CAMERA_OFFSETX = -256.0f;
	const float CAMERA_OFFSETY = -224.0f;
	const std::string walkAnims[4] = { "WalkRight", "WalkUp", "WalkLeft", "WalkDown" };
	const std::string standAnims[4] = { "StandRight", "StandUp", "StandLeft", "StandDown" };
	const std::string attackAnims[4] = { "AttackRight", "AttackUp", "AttackLeft", "AttackDown" };
	Vector2 moveDir;
	MoveDir moveDirName;
	MoveDir facingDir;
	bool lastAttackInput;
	float attackTimer;
	Sword* sword;
	void UpdateSword();
};