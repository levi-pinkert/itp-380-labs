#include "Actor.h"
class Game;
class AnimatedSprite;
class CollisionComponent;
class PlayerMove;

class Player : public Actor {
public:
	Player(Game* game);
	virtual ~Player();
private:
	const float COLLISION_SIZE = 20.0f;
	AnimatedSprite* spriteComp;
	CollisionComponent* collisionComp;
	PlayerMove* moveComp;
};