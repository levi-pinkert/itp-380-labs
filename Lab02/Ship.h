#include "Actor.h"
class MoveComponent;
class SpriteComponent;

class Ship : public Actor {
public:
	Ship(Game*);
	virtual void OnProcessInput(const Uint8* keyState) override;
	virtual void OnUpdate(float deltaTime) override;
private:
	MoveComponent* moveComp;
	SpriteComponent* spriteComp;
	int thrustInput;
	int steerInput;
	bool shootInput;
	float shootCooldown;
	const float THRUST_SPEED = 300.0f;
	const float STEER_SPEED = 1.2f * Math::Pi;
	const float SHOOT_SPEED = 1.0f;
};