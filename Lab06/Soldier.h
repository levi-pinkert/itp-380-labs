#include "Actor.h"
class Game;
class AnimatedSprite;
class CollisionComponent;
class PathNode;
class SoldierAI;
class EnemyComponent;

class Soldier : public Actor {
public:
	Soldier(Game* game, PathNode* start, PathNode* end);
private:
	AnimatedSprite* spriteComp;
	CollisionComponent* collisionComp;
	SoldierAI* aiComp;
	EnemyComponent* enemyComp;
};