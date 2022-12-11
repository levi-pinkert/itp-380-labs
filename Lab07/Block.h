#include "Actor.h"
class Game;
class CollisionComponent;

class Block : public Actor {
public:
	Block(Game* game, bool exp);
	virtual ~Block();
	virtual void OnUpdate(float deltaTime);
	CollisionComponent* GetCollision();
	void Explode();
	bool IsExplosive();
private:
	const float SCALE = 25.0f;
	const float DESTROY_DIST = 2000.0f;
	const float EXPLOSION_RADIUS = 50.0f;
	CollisionComponent* collisionComp;
	bool explosive;
};