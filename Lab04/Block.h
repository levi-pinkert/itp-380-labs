#include "Actor.h"
class Game;
class SpriteComponent;
class CollisionComponent;

class Block : public Actor {
public:
	Block(Game* game, char type);
	virtual ~Block();
	CollisionComponent* GetCollisionComponent();
private:
	const float SIZE = 32.0f;
	SpriteComponent* spriteComp;
	CollisionComponent* collisionComp;
};