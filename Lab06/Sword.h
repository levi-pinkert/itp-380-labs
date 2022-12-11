#include "Actor.h"
class Game;
class CollisionComponent;

class Sword : public Actor {
public:
	Sword(Game* game);
	CollisionComponent* GetCollisionComp();
private:
	CollisionComponent* collisionComp;
};