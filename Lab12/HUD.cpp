#include "HUD.h"
#include "UIComponent.h"
#include "Actor.h"
#include <SDL2/SDL_ttf.h>
#include "Shader.h"
#include "Font.h"
#include "Texture.h"
#include <string>
#include <sstream>
#include "Game.h"
#include "Renderer.h"
#include "SecurityCamera.h"

HUD::HUD(Actor* owner) : UIComponent(owner) {
	mFont = new Font();
	mFont->Load("Assets/Inconsolata-Regular.ttf");
	mTimerText = mFont->RenderText(GetTimerText(0.0f));
	mCoinsText = mFont->RenderText(GetCoinsText(0));
	Renderer* renderer = mOwner->GetGame()->GetRenderer();
	mRadarBackground = renderer->GetTexture("Assets/Radar.png");
	mRadarArrow = renderer->GetTexture("Assets/RadarArrow.png");;
	mRadarBlip = renderer->GetTexture("Assets/Blip.png");;
}

HUD::~HUD() {
	mFont->Unload();
	delete mFont;
}

void HUD::Update(float deltaTime) {
	mTimerText->Unload();
	mTimerText = mFont->RenderText(GetTimerText(mOwner->GetGame()->GetTimer()));
	mCoinsText->Unload();
	mCoinsText = mFont->RenderText(GetCoinsText(mOwner->GetGame()->GetCoins()));
}

void HUD::Draw(Shader* shader) {
	//Draw timer and coins
	DrawTexture(shader, mTimerText, TIMER_SCREEN_POS);
	DrawTexture(shader, mCoinsText, COINS_SCREEN_POS);

	//Draw checkpoint text
	Texture* checkpointText = mOwner->GetGame()->GetCheckpointText();
	if (checkpointText != nullptr) {
		DrawTexture(shader, checkpointText, Vector2::Zero);
	}

	//Draw radar
	DrawTexture(shader, mRadarBackground, RADAR_SCREEN_POS);
	DrawTexture(shader, mRadarArrow, RADAR_SCREEN_POS);
	Vector3 fwd = mOwner->GetForward();
	float fwdAngle = Math::Atan2(fwd.y, fwd.x);
	Matrix3 rotationMat = Matrix3::CreateRotation(fwdAngle);
	Vector3 pos = mOwner->GetPosition();
	Vector2 xyPos(pos.y, pos.x);
	auto cameras = mOwner->GetGame()->GetSecurityCameras();
	for (SecurityCamera* cam : cameras) {
		Vector3 camPos = cam->GetPosition();
		Vector2 xyCamPos(camPos.y, camPos.x);
		Vector2 playerToCam = xyCamPos - xyPos;
		if (playerToCam.LengthSq() <= RADAR_RANGE * RADAR_RANGE) {
			playerToCam *= RADAR_SCREEN_RADIUS / RADAR_RANGE;
			Vector2 screenPos = RADAR_SCREEN_POS + Vector2::Transform(playerToCam, rotationMat);
			Vector3 camFwd = cam->GetWorldForward();
			Vector2 xyCamFwd(camFwd.y, camFwd.x);
			xyCamFwd = Vector2::Transform(xyCamFwd, rotationMat);
			float camFwdAngle = Math::Atan2(xyCamFwd.y, xyCamFwd.x);
			DrawTexture(shader, mRadarBlip, screenPos, 1.0f, camFwdAngle);
		}
	}
}

void HUD::SetCoins(int coins) {
	
}

std::string HUD::GetTimerText(float timer) {
	int min = (int)timer / 60;
	int sec = (int)timer % 60;
	int ms = (int)(timer * 100.0f) % 100;
	std::stringstream res;
	if (min < 10) { res << "0"; }
	res << min << ":";
	if (sec < 10) { res << "0"; }
	res << sec << ":";
	if (ms < 10) { res << "0"; }
	res << ms;
	return res.str();
}

std::string HUD::GetCoinsText(int coins) {
	std::stringstream res;
	if (coins < 10) { res << "0"; }
	res << coins << "/" << MAX_COINS;
	return res.str();
}

void HUD::SetCheckpointText(std::string text) {
	mOwner->GetGame()->SetCheckpointText(mFont->RenderText(text));
}