#include "Component.h"
#include <functional>
class Actor;
class CollisionComponent;

class EnemyComponent : public Component {
public:
	EnemyComponent(Actor* owner);
	virtual ~EnemyComponent();
	CollisionComponent* GetCollisionComp();
	int GetHitPoints();
	void SetHitPoints(int newHitPoints);
	void TakeDamage();
	virtual void Update(float deltaTime);
	void SetOnDamage(std::function<void()> callback) { onDamageCallback = callback; }
	void SetOnDeath(std::function<void()> callback) { onDeathCallback = callback; }
private:
	const float INVINCIBILITY_TIME = 0.25f;
	CollisionComponent* collisionComp;
	int hitPoints;
	float invincibilityTimer;
	std::function<void()> onDamageCallback;
	std::function<void()> onDeathCallback;
};