#include "Actor.h"
class Game;

class SecurityCamera : public Actor {
public:
	SecurityCamera(Game* game, Actor* parent);
	virtual ~SecurityCamera();
	void SetStartQ(Quaternion q) { startQ = q; }
	void SetEndQ(Quaternion q) { endQ = q; }
	void SetInterpTime(float t) { interpTime = t; }
	void SetPauseTime(float t) { pauseTime = t; }
	void SetPaused(bool p) { paused = p; }
protected:
	virtual void OnUpdate(float deltaTime) override;
private:
	const float MAX_SOUND_DIST = 1500.0f;
	const float MIN_SOUND_DIST = 500.0f;
	Quaternion startQ = Quaternion::Identity;
	Quaternion endQ = Quaternion::Identity;
	float interpTime = 1.0f;
	float pauseTime = 1.0f;
	float moveTimer = 0.0f;
	bool paused = false;
	int motorSoundChannel = -1;
	int GetSoundVolume();
};