#include "Actor.h"
class MoveComponent;
class SpriteComponent;

class Asteroid : public Actor {
public:
	Asteroid(Game*);
	virtual ~Asteroid() override;
	virtual void OnUpdate(float deltaTime) override;
private:
	MoveComponent* moveComp;
	SpriteComponent* spriteComp;
	const float SPEED = 150.0f;
};