#include "SecurityCone.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Player.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Math.h"
#include "Actor.h"
#include "SecurityCamera.h"

SecurityCone::SecurityCone(Game* game, Actor* parent) : Actor(game, parent) {
	SetPosition(DEFAULT_OFFSET);
	SetScale(SCALE);
	meshComp = new MeshComponent(this, true);
	meshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Cone.gpmesh"));
}

void SecurityCone::OnUpdate(float deltaTime) {
	Vector3 playerPos = mGame->GetPlayer()->GetWorldPosition();
	if (detectTime >= TIME_TO_DETECT || PointInCone(playerPos)) {
		if (detectTime == 0.0f) {
			detectSoundChannel = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/SecurityDetected.wav"), 0);
		}
		detectTime += deltaTime;
		if (detectTime >= TIME_TO_KILL) {
			mGame->GetPlayer()->Die();
			detectTime = 0.0f;
		}
	}
	else {
		if (detectSoundChannel != -1) {
			Mix_HaltChannel(detectSoundChannel);
			detectSoundChannel = -1;
		}
		detectTime = 0.0f;
	}
	((SecurityCamera*)mParent)->SetPaused(detectTime > 0.0f);
	meshComp->SetTextureIndex(detectTime == 0.0f ? 0 : (detectTime >= TIME_TO_DETECT ? 2 : 1));
}

bool SecurityCone::PointInCone(Vector3 point) {
	//Check for distance
	Vector3 fwd = GetWorldForward();
	Vector3 pos = GetWorldPosition();
	float dist = Vector3::Dot(fwd, point - pos);
	float scale = GetScale();
	if (dist > scale * CONE_LEN || dist <= 0.0f) { return false; }

	//Check for angle
	Vector3 dirToPoint = point - pos;
	dirToPoint.Normalize();
	float theta = Math::Acos(Vector3::Dot(fwd, dirToPoint));
	return theta <= HALF_CONE_ANGLE;
}