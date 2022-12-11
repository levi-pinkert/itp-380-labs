#include "Actor.h"
class Game;
class MeshComponent;
class CollisionComponent;

class Block : public Actor {
public:
	Block(Game* game);
	virtual ~Block();
private:
	const float SCALE = 64.0f;
	MeshComponent* meshComp;
	CollisionComponent* collisionComp;
};