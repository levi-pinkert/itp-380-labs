#include "Actor.h"
#include <string>
class Game;
class MeshComponent;
class CollisionComponent;

class Checkpoint : public Actor {
public:
	Checkpoint(Game* game, Actor* owner);
	void SetActive(bool newActive);
	bool GetActive();
	void SetLevelString(std::string levelString) { mLevelString = levelString; }
protected:
	virtual void OnUpdate(float deltaTime) override;
private:
	const float SIZE = 25.0f;
	bool active = false;
	MeshComponent* meshComp;
	std::string mLevelString;
	CollisionComponent* collisionComp;
};