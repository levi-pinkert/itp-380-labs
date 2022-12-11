#include "Actor.h"
class CollisionComponent;
class Game;

class Collider : public Actor {
public:
	Collider(Game* game, float width, float height);
	CollisionComponent* getCollisionComp();
private:
	CollisionComponent* collisionComp;
};