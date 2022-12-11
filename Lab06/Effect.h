#include "Actor.h"
#include "Math.h"
#include <string>
class Game;
class AnimatedSprite;

class Effect : public Actor {
public:
	Effect(Game* game, Vector2 pos, std::string animationName, std::string soundName);
	virtual void OnUpdate(float deltaTime);
private:
	float lifetime;
};