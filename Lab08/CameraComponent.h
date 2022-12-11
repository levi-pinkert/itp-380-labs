#include "Component.h"
#include "Math.h"
class Actor;

class CameraComponent : public Component {
public:
	CameraComponent(Actor* owner);
	virtual void Update(float deltaTime);
	void SnapToIdeal();
private:
	const float H_DIST = 60.0f;
	const float Z_POS = 70.0f;
	const float TARGET_DIST = 50.0f;
	const float SPRING = 256.0f;
	const float DAMPENING = 2.0f * Math::Sqrt(SPRING);
	Vector3 position;
	Vector3 velocity;
	Vector3 CalculateIdeal();
	void UpdateViewMatrix();
};