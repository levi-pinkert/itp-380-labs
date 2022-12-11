#include "Component.h"
#include "Math.h"
class Actor;

class CameraComponent : public Component {
public:
	CameraComponent(Actor* owner);
	virtual void Update(float deltaTime);
	void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
	float GetPitchSpeed() { return mPitchSpeed; }
private:
	float mPitchAngle;
	float mPitchSpeed;
};