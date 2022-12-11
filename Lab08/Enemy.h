#include "Actor.h"

class Game;
class EnemyMove;

class Enemy : public Actor {
public:
	Enemy(Game* game);
private:
	EnemyMove* moveComp;
};