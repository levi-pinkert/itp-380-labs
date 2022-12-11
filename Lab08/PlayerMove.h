#include "Math.h"
#include "VehicleMove.h"
class Actor;
class Player;

class PlayerMove : public VehicleMove {
public:
	PlayerMove(Actor* actor);
	virtual void OnLapChange(int newLap) override;
	virtual void Update(float deltaTime) override;
	virtual void ProcessInput(const Uint8* keyState) override;	
private:
	Player* player;
	const int START_CELLROW = 39;
	const int START_CELLCOL = 58;
	const int MAX_LAPS = 5;
};