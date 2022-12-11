#include "VehicleMove.h"
#include "Actor.h"
#include "SDL2/SDL.h"
#include "Game.h"
#include "HeightMap.h"
#include <fstream>
#include "CSVHelper.h"

VehicleMove::VehicleMove(Actor* owner) :
	Component(owner, 50),
	gasPressed(false),
	steeringInput(SteerDir::NONE),
	angularVelocity(0.0f),
	gasPressDuration(0.0f),
	currentLap(0),
	lastCheckpoint(-1)
{
	//Load the checkpoint data
	std::fstream file;
	file.open(CHECKPOINT_FILE);
	if (file.is_open()) {
		//Throw out the first line
		std::string line;
		std::getline(file, line);

		//Parse each row into a vector and push it onto the end of the heightMap
		while (!file.eof()) {
			std::getline(file, line);
			if (line.empty()) { continue; }
			std::vector<std::string> strs = CSVHelper::Split(line);
			if (strs.size() == 5) {
				Checkpoint cp;
				cp.minX = std::stoi(strs[1]);
				cp.maxX = std::stoi(strs[2]);
				cp.minY = std::stoi(strs[3]);
				cp.maxY = std::stoi(strs[4]);
				checkpoints.push_back(cp);
			}
		}
	}
}

void VehicleMove::Update(float deltaTime) {
	//Accelerate
	if (gasPressed) {
		gasPressDuration += deltaTime;
		float accel = Math::Lerp(MIN_ACCEL, MAX_ACCEL, Math::Clamp(gasPressDuration / ACCEL_RAMP_TIME, 0.0f, 1.0f));
		velocity += mOwner->GetForward() * accel * deltaTime;
	}
	else {
		gasPressDuration = 0.0f;
	}

	//Update position
	mOwner->SetPosition(mOwner->GetPosition() + velocity * deltaTime);

	//Apply linear drag
	velocity *= gasPressed ? LINEAR_DRAG : LIFT_LINEAR_DRAG;

	//Update angular velocity
	float turnFactor = steeringInput == SteerDir::RIGHT ? 1.0f : (steeringInput == SteerDir::LEFT ? -1.0f : 0.0f);
	angularVelocity += ANG_ACCEL * turnFactor * deltaTime;

	//Update angle
	mOwner->SetRotation(mOwner->GetRotation() + angularVelocity * deltaTime);

	//Apply angular drag
	angularVelocity *= ANG_DRAG;

	//Set Height
	Vector2 flatPos(mOwner->GetPosition().x, mOwner->GetPosition().y);
	HeightMap* heightMap = mOwner->GetGame()->GetHeightMap();
	if (heightMap->IsOnTrack(flatPos)) {
		float goalHeight = heightMap->GetHeight(flatPos);
		mOwner->SetPosition(Vector3(flatPos.x, flatPos.y, Math::Lerp(mOwner->GetPosition().z, goalHeight, H_LERP_SPD)));
	}

	//Handle laps and checkpoints
	CheckCheckpoint();
}

void VehicleMove::CheckCheckpoint() {
	int nextCheckpoint = (lastCheckpoint + 1) % checkpoints.size();
	Checkpoint goalCp = checkpoints[nextCheckpoint];
	Vector2 cellPos = mOwner->GetGame()->GetHeightMap()->WorldToCell(Vector2(mOwner->GetPosition().x, mOwner->GetPosition().y));
	int cellX = (int)cellPos.x;
	int cellY = (int)cellPos.y;
	if (cellX >= goalCp.minX && cellX <= goalCp.maxX && cellY >= goalCp.minY && cellY <= goalCp.maxY) {
		//Check if we just started a new lap
		if (nextCheckpoint == 0) {
			currentLap++;
			OnLapChange(currentLap);
		}

		//We've reached the next checkpoint
		lastCheckpoint = nextCheckpoint;
	}
}

int VehicleMove::GetLastCheckpoint() {
	return lastCheckpoint;
}

int VehicleMove::GetLap() {
	return currentLap;
}

float VehicleMove::GetDistanceToCheckpoint() {
	Checkpoint goalCp = checkpoints[(lastCheckpoint + 1) % checkpoints.size()];
	HeightMap* heightMap = mOwner->GetGame()->GetHeightMap();
	Vector3 minCp = heightMap->CellToWorld(goalCp.minX, goalCp.minY);
	Vector3 maxCp = heightMap->CellToWorld(goalCp.maxX, goalCp.maxY);
	Vector3 middleCp = Vector3::Lerp(minCp, maxCp, 0.5);
	return Vector3::Distance(mOwner->GetPosition(), middleCp);
}

void VehicleMove::SetGasInput(bool input) {
	gasPressed = input;
}

void VehicleMove::SetSteeringInput(SteerDir input) {
	steeringInput = input;
}