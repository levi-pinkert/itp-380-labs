#include "Actor.h"
class Game;
class AnimatedSprite;
class CollisionComponent;
class GoombaMove;

class Goomba : public Actor {
public:
	Goomba(Game* game);
	virtual ~Goomba();
	virtual void OnUpdate(float deltaTime);
	CollisionComponent* GetCollisionComponent();
	void Stomp();
	bool GetStomped();
private:
	const float SIZE = 32.0f;
	const float DESPAWN_TIME = 0.25f;
	AnimatedSprite* spriteComp;
	CollisionComponent* collisionComp;
	GoombaMove* moveComp;
	bool stomped;
	float despawnTimer;
};