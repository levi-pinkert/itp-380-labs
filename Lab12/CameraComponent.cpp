#include "CameraComponent.h"
#include "Math.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

CameraComponent::CameraComponent(Actor* owner) : Component(owner) {

}

void CameraComponent::Update(float deltaTime) {
	//Apply pitch speed
	mPitchAngle = Math::Clamp(mPitchAngle + mPitchSpeed * deltaTime, -Math::Pi / 4.0f, Math::Pi / 4.0f);

	//Calculate forward
	Matrix4 rotationMat = Matrix4::CreateRotationY(mPitchAngle) * Matrix4::CreateRotationZ(mOwner->GetRotation());
	Vector3 forward = Vector3::Transform(Vector3::UnitX, rotationMat);

	//Update up (based on wall run)
	Vector3 goalUp = Vector3::UnitZ + mWallRunNormal * ROLL_AMOUNT;
	if (mUp.x < goalUp.x) {
		mUp.x = Math::Min(mUp.x + ROLL_SPD * deltaTime, goalUp.x);
	}
	if (mUp.x > goalUp.x) {
		mUp.x = Math::Max(mUp.x - ROLL_SPD * deltaTime, goalUp.x);
	}
	if (mUp.y < goalUp.y) {
		mUp.y = Math::Min(mUp.y + ROLL_SPD * deltaTime, goalUp.y);
	}
	if (mUp.y > goalUp.y) {
		mUp.y = Math::Max(mUp.y - ROLL_SPD * deltaTime, goalUp.y);
	}

	//Set view matrix
	Vector3 ownerPos = mOwner->GetPosition();
	Matrix4 viewMat = Matrix4::CreateLookAt(ownerPos, ownerPos + forward, mUp);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(viewMat);
}
