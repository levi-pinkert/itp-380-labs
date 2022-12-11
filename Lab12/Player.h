#include "Actor.h"
class Game;
class MeshComponent;
class PlayerMove;
class CameraComponent;
class CollisionComponent;

class Player : public Actor {
public:
	Player(Game* game, Actor* parent);
	void SetRespawnPos(Vector3 newRespawnPos) { respawnPosition = newRespawnPos; }
	Vector3 GetRespawnPos() { return respawnPosition; }
	void Die();
protected:
	virtual void OnUpdate(float deltaTime);
private:
	const float COLL_WIDTH = 50.0f;
	const float COLL_HEIGHT = 175.0f;
	const float COLL_DEPTH = 50.0f;
	PlayerMove* moveComp;
	CameraComponent* cameraComp;
	CollisionComponent* collisionComp;
	Vector3 respawnPosition;
};