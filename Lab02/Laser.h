#include "Actor.h"
class MoveComponent;
class SpriteComponent;

class Laser : public Actor {
public:
	Laser(Game*);
	virtual void OnUpdate(float deltaTime) override;
private:
	MoveComponent* moveComp;
	SpriteComponent* spriteComp;
	float lifetime;
	const float SPEED = 400.0f;
	const float MAX_LIFETIME = 1.0f;
};