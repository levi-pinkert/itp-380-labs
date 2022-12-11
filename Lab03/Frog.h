#include "Actor.h"
#include <map>
#include "SDL2/SDL.h"
#include "Math.h"
class MoveComponent;
class SpriteComponent;
class CollisionComponent;

class Frog : public Actor {
public:
	Frog(Game*);
	virtual ~Frog();
	virtual void OnProcessInput(const Uint8* keyState) override;
	virtual void OnUpdate(float deltaTime) override;
private:
	const int MOVE_DIST = 64;
	const Vector2 RESPAWN_POS = Vector2(448.0f, 928.0f);
	const float WATER_TOP = 180.0f;
	const float WATER_BOTTOM = 510.0f;
	const float LOG_X_ADJUST = 32.0f;
	SpriteComponent* spriteComp;
	CollisionComponent* collisionComp;
	std::map<SDL_Scancode, bool> lastFrameInput;
	void Die();
};