#include "Actor.h"
class Game;
class MeshComponent;
class PlayerMove;
class CameraComponent;
class PlayerUI;

class Player : public Actor {
public:
	Player(Game* game);
protected:
	virtual void OnUpdate(float deltaTime);
private:
	MeshComponent* meshComp;
	PlayerMove* moveComp;
	CameraComponent* cameraComp;
	PlayerUI* uiComp;
};