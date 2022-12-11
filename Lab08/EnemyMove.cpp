#include "EnemyMove.h"
#include "Actor.h"
#include "Game.h"
#include "HeightMap.h"
#include <fstream>
#include "CSVHelper.h"
#include "Math.h"

EnemyMove::EnemyMove(Actor* owner) : VehicleMove(owner) {
	//Initialize Member Variables
	targetIndex = 1;

	//Load the path from csv
	std::fstream file;
	file.open(PATH_FILE);
	HeightMap* heightMap = owner->GetGame()->GetHeightMap();
	if (file.is_open()) {
		//Parse a position out of each row
		std::string line;
		std::getline(file, line);	//throw out the first line
		while (!file.eof()) {
			std::getline(file, line);
			std::vector<std::string> strs = CSVHelper::Split(line);
			if (strs.size() == 3) {
				int cellX = std::stoi(strs[1]);
				int cellY = std::stoi(strs[2]);
				route.push_back(heightMap->CellToWorld(cellX, cellY));
			}
		}
	}

	//Initialize position to first path point
	mOwner->SetPosition(route[0]);
}

void EnemyMove::Update(float deltaTime) {
	//Check if the AI should target the next point
	if (Vector3::Distance(mOwner->GetPosition(), route[targetIndex]) < TARGET_DIST) {
		targetIndex = (targetIndex + 1) % route.size();
	}
	
	//Accelerate if facing target
	Vector3 toTarget = route[targetIndex] - mOwner->GetPosition();
	Vector3 forward = mOwner->GetForward();
	float dot = Vector3::Dot(toTarget, forward);
	float theta = Math::Acos(dot / toTarget.Length());
	SetGasInput(theta <= ACCEL_ANGLE);

	//Turn to face target
	if (theta > TURN_DEADZONE) {
		Vector3 cross = Vector3::Cross(forward, toTarget);
		SetSteeringInput(cross.z > 0.0f ? SteerDir::RIGHT : SteerDir::LEFT);
	}
	else {
		SetSteeringInput(SteerDir::NONE);
	}

	//Do the movement code
	VehicleMove::Update(deltaTime);
}
