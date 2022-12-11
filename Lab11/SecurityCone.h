#include "Actor.h"
class Game;
class MeshComponent;

class SecurityCone : public Actor {
public:
	SecurityCone(Game* game, Actor* parent);
protected:
	virtual void OnUpdate(float deltaTime) override;
private:
	const Vector3 DEFAULT_OFFSET = Vector3(30.0f, 0.0f, 0.0f);
	const float SCALE = 3.0f;
	const float CONE_LEN = 100.0f;
	const float HALF_CONE_ANGLE = Math::Pi / 6.0f;
	const float TIME_TO_DETECT = 2.0f;
	const float TIME_TO_KILL = 2.5f;
	float detectTime = 0.0f;
	MeshComponent* meshComp;
	bool PointInCone(Vector3 point);
};