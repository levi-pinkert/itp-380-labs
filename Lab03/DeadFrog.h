#include "Actor.h"
#include "Math.h"
class SpriteComponent;

class DeadFrog : public Actor {
public:
	DeadFrog(Game*);
	virtual void OnUpdate(float deltaTime) override;
private:
	const float DESPAWN_TIME = 0.5f;
	SpriteComponent* spriteComp;
	float timer;
};