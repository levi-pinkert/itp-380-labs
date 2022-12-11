#include "Component.h"
#include "Math.h"
class Actor;

class CameraComponent : public Component {
public:
	CameraComponent(Actor* owner);
	virtual void Update(float deltaTime);
	void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
	float GetPitchSpeed() { return mPitchSpeed; }
	void SetWallRunNormal(Vector3 normal) { mWallRunNormal = normal; }
private:
	const float ROLL_AMOUNT = 0.5f;
	const float ROLL_SPD = 1.5f;
	float mPitchAngle = 0.0f;
	float mPitchSpeed = 0.0f;
	Vector3 mWallRunNormal = Vector3::Zero;
	Vector3 mUp = Vector3::UnitZ;
};