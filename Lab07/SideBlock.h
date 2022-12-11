#include "Actor.h"
class Game;

class SideBlock : public Actor {
public:
	SideBlock(Game* game, size_t textureIndex);
	virtual void OnUpdate(float deltaTime);
private:
	const float DESTROY_DIST = 2000.0f;
};