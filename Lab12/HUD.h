#include "UIComponent.h"
#include <string>
class Actor;
class Font;

class HUD : public UIComponent {
public:
	HUD(Actor* owner);
	~HUD();
	virtual void Update(float deltaTime) override;
	virtual void Draw(class Shader* shader) override;
	void SetCoins(int coins);
	void SetCheckpointText(std::string text);
private:
	const int MAX_COINS = 55;
	const Vector2 RADAR_SCREEN_POS = Vector2(400.0f, -270.0f);
	const Vector2 TIMER_SCREEN_POS = Vector2(-420.0f, -325.0f);
	const Vector2 COINS_SCREEN_POS = Vector2(-441.0f, -290.0f);
	const float RADAR_RANGE = 1500.0f;
	const float RADAR_SCREEN_RADIUS = 92.0f;
	Font* mFont = nullptr;
	Texture* mTimerText = nullptr;
	Texture* mCoinsText = nullptr;
	Texture* mRadarBackground = nullptr;
	Texture* mRadarArrow = nullptr;
	Texture* mRadarBlip = nullptr;
	std::string GetTimerText(float timer);
	std::string GetCoinsText(int coins);
};