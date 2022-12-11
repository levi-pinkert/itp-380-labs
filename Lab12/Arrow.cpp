#include "Arrow.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Checkpoint.h"
#include "Player.h"

Arrow::Arrow(Game* game, Actor* owner) : Actor(game, owner) {
	MeshComponent* meshComp = new MeshComponent(this);
	meshComp->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Arrow.gpmesh"));
	SetScale(SCALE);
}

void Arrow::OnUpdate(float deltaTime) {
	//Update rotation
	Checkpoint* cp = mGame->GetActiveCheckpoint();
	Quaternion quat;
	if (cp == nullptr) {
		quat = Quaternion::Identity;
	}
	else {
		Vector3 dir = cp->GetPosition() - mGame->GetPlayer()->GetPosition();
		if (dir.LengthSq() == 0.0f) {
			quat = Quaternion::Identity;
		}
		else {
			dir.Normalize();
			float rotTheta = Math::Acos(Vector3::Dot(Vector3::UnitX, dir));
			Vector3 rotAxis = Vector3::Cross(Vector3::UnitX, dir);
			float rotDot = Vector3::Dot(Vector3::UnitX, dir);
			if (rotDot == 1.0f) {
				quat = Quaternion::Identity;
			}
			else if (rotDot == -1.0f) {
				quat = Quaternion(Vector3::UnitZ, Math::Pi);
			}
			else {
				rotAxis.Normalize();
				quat = Quaternion(rotAxis, rotTheta);
			}
		}
	}
	SetQuaternion(quat);

	//Update position
	SetPosition(mGame->GetRenderer()->Unproject(mScreenPos));
}