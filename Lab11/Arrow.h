#include "Actor.h"
class Game;

class Arrow : public Actor {
public:
	Arrow(Game* game, Actor* owner);
protected:
	virtual void OnUpdate(float deltaTime) override;
private:
	const float SCALE = 0.15f;
	const float CLOSE_THRESH = 0.0001f;
	const Vector3 mScreenPos = Vector3(0.0f, 250.0f, 0.1f);
};