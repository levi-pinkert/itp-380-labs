#include "SecurityCamera.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Player.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Math.h"
#include "Actor.h"
#include "SecurityCone.h"

SecurityCamera::SecurityCamera(Game* game, Actor* parent) : Actor(game, parent) {
	MeshComponent* meshComp = new MeshComponent(this);
	meshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Camera.gpmesh"));
	new SecurityCone(mGame, this);
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
}