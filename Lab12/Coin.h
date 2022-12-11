#include "Actor.h"
class Game;
class CollisionComponent;

class Coin : public Actor {
public:
	Coin(Game* game, Actor* owner);
protected:
	virtual void OnUpdate(float deltaTime) override;
private:
	const float ROT_SPD = Math::Pi;
	const float SIZE = 100.0f;
	CollisionComponent* collisionComp;
};