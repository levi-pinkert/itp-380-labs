#include "Actor.h"
class Game;
class MeshComponent;
class CollisionComponent;

class Block : public Actor {
public:
	Block(Game* game, Actor* parent);
	virtual ~Block();
	bool GetIsMirror() { return isMirror; }
	void SetIsMirror(bool newIsMirror) { isMirror = newIsMirror; }
	bool GetRotates() { return rotates; }
	void SetRotates(bool newRotates) { rotates = newRotates; }
protected:
	virtual void OnUpdate(float deltaTime) override;
private:
	const float SCALE = 64.0f;
	const float ROT_SPD = Math::Pi / 4.0f;
	MeshComponent* meshComp;
	CollisionComponent* collisionComp;
	bool isMirror = false;
	bool rotates = false;
};