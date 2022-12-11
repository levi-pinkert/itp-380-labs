#include "Actor.h"
class Game;
class CollisionComponent;

class Bullet : public Actor {
public:
	Bullet(Game* game, float speedMultiplier);
	virtual void OnUpdate(float deltaTime);
private:
	const float SCALE = 5.0f;
	const float SPEED = 900.0f;
	const float COLL_SIZE = 10.0f;
	const float MAX_LIFETIME = 1.0f;
	CollisionComponent* collisionComp;
	float lifetime;
};