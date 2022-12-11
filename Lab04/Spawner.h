#include "Actor.h"
class Game;

class Spawner : public Actor {
public:
	Spawner(Game* game);
	virtual void OnUpdate(float deltaTime);
private:
	const float SPAWN_DIST = 600.0f;
};