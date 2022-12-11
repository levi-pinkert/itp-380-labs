#include "VehicleMove.h"
#include <vector>
#include <string>
#include "Math.h"

class Actor;

class EnemyMove : public VehicleMove {
public:
	EnemyMove(Actor* owner);
	virtual void Update(float deltaTime) override;
private:
	const std::string PATH_FILE = "Assets/HeightMap/Path.csv";
	const float TARGET_DIST = 100.0f;
	const float TURN_DEADZONE = Math::Pi / 36.0f;
	const float ACCEL_ANGLE = Math::Pi;
	std::vector<Vector3> route;
	size_t targetIndex;
};