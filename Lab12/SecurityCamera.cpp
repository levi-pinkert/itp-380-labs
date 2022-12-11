#include "SecurityCamera.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Player.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Math.h"
#include "Actor.h"
#include "SecurityCone.h"
#include <vector>

SecurityCamera::SecurityCamera(Game* game, Actor* parent) : Actor(game, parent) {
	MeshComponent* meshComp = new MeshComponent(this);
	meshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Camera.gpmesh"));
	new SecurityCone(mGame, this);
	mGame->GetSecurityCameras().push_back(this);
}

SecurityCamera::~SecurityCamera() {
	//halt sound
	if (motorSoundChannel != -1) {
		Mix_HaltChannel(motorSoundChannel);
		motorSoundChannel = -1;
	}

	//remove self from list of security cameras
	std::vector<SecurityCamera*>& securityCameras = mGame->GetSecurityCameras();
	std::vector<SecurityCamera*>::iterator camerasIt = securityCameras.begin();
	while (camerasIt != securityCameras.end()) {
		if (*camerasIt == this) {
			securityCameras.erase(camerasIt);
			break;
		}
	}
}

void SecurityCamera::OnUpdate(float deltaTime) {
	if (!paused) {
		//Advance timer
		moveTimer += deltaTime;

		//Set quaternion
		float progress = Math::Clamp(moveTimer / interpTime, 0.0f, 1.0f);
		SetQuaternion(Quaternion::Slerp(startQ, endQ, progress));

		//Check if it should start going again
		if (moveTimer > interpTime + pauseTime) {
			moveTimer = 0.0f;
			std::swap(startQ, endQ);
		}
	}

	//Start or stop sounds
	if (!paused && moveTimer <= interpTime) {
		//Sound should be going
		if (motorSoundChannel == -1) {
			motorSoundChannel = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotor.wav"), 0);
		}
	}
	else {
		//Sound should be stopped
		if (motorSoundChannel != -1) {
			Mix_HaltChannel(motorSoundChannel);
			motorSoundChannel = -1;
			int stopSoundChannel = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotorStop.wav"), 0);
			Mix_Volume(stopSoundChannel, GetSoundVolume());
		}
	}

	//Update motor volume
	if (motorSoundChannel != -1) {
		Mix_Volume(motorSoundChannel, GetSoundVolume());
	}
}

int SecurityCamera::GetSoundVolume() {
	float dist = Vector3::Distance(mPosition, mGame->GetPlayer()->GetPosition());
	float volFactor = 1.0f - Math::Clamp((dist - MIN_SOUND_DIST) / (MAX_SOUND_DIST - MIN_SOUND_DIST), 0.0f, 1.0f);
	return (int)(volFactor * 128.0f);
}