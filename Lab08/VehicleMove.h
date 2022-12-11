#ifndef VEHICLEMOVE_H
#define VEHICLEMOVE_H
#include "Component.h"
#include "Math.h"
#include <vector>
#include <string>
class Actor;


class VehicleMove : public Component {
public:
	VehicleMove(Actor* owner);
	virtual void Update(float deltaTime);
	void SetGasInput(bool input);
	enum SteerDir { LEFT, RIGHT, NONE };
	void SetSteeringInput(SteerDir input);
	virtual void OnLapChange(int newLap) { }
	int GetLastCheckpoint();
	int GetLap();
	float GetDistanceToCheckpoint();
private:
	const float MIN_ACCEL = 1000.0f;
	const float MAX_ACCEL = 2500.0f;
	const float ACCEL_RAMP_TIME = 2.0f;
	const float ANG_ACCEL = 5.0f * Math::Pi;
	const float LINEAR_DRAG = 0.9f;
	const float LIFT_LINEAR_DRAG = 0.975f;
	const float ANG_DRAG = 0.9f;
	const float H_LERP_SPD = 0.1f;
	const std::string CHECKPOINT_FILE = "Assets/HeightMap/Checkpoints.csv";
	bool gasPressed;
	SteerDir steeringInput;
	Vector3 velocity;
	float angularVelocity;
	float gasPressDuration;
	struct Checkpoint {
		int minX;
		int maxX;
		int minY;
		int maxY;
	};
	std::vector<Checkpoint> checkpoints;
	int currentLap;
	int lastCheckpoint;
	void CheckCheckpoint();
};
#endif