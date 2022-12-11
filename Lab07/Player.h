#include "Actor.h"
class Game;
class MeshComponent;
class CollisionComponent;
class PlayerMove;
class HUD;

class Player : public Actor {
public:
	Player(Game* game);
	CollisionComponent* GetCollisionComponent();
	void TakeDamage();
	void Heal();
	int GetHitPoints();
	virtual void OnUpdate(float deltaTime);
private:
	const int INIT_SHIELD = 3;
	const float COLL_X = 40.0f, COLL_Y = 25.0f, COLL_Z = 15.0f;
	const float PEPPY_COOLDOWN_MIN = 15.0f, PEPPY_COOLDOWN_MAX = 25.0f;
	MeshComponent* meshComp;
	CollisionComponent* collisionComp;
	PlayerMove* moveComp;
	HUD* hud;
	int shieldLevel;
	float peppyCooldown;
	int shipLoopChannel;
	int damageAlertChannel;
};