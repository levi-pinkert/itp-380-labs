#include "Actor.h"
class Game;
class SpriteComponent;
class CollisionComponent;
class EnemyComponent;

class Bush : public Actor {
public:
	Bush(Game* game);
private:
	SpriteComponent* spriteComp;
	CollisionComponent* collisionComp;
	EnemyComponent* enemyComp;
};