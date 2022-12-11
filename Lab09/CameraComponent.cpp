#include "CameraComponent.h"
#include "Math.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

CameraComponent::CameraComponent(Actor* owner) : Component(owner), mPitchAngle(0.0f), mPitchSpeed(0.0f) {

}

void CameraComponent::Update(float deltaTime) {
	//Apply pitch speed
	mPitchAngle = Math::Clamp(mPitchAngle + mPitchSpeed * deltaTime, -Math::Pi / 4.0f, Math::Pi / 4.0f);

	//Calculate forward
	Matrix4 rotationMat = Matrix4::CreateRotationY(mPitchAngle) * Matrix4::CreateRotationZ(mOwner->GetRotation());
	Vector3 forward = Vector3::Transform(Vector3::UnitX, rotationMat);

	//Set view matrix
	Vector3 ownerPos = mOwner->GetPosition();
	Matrix4 viewMat = Matrix4::CreateLookAt(ownerPos, ownerPos + forward, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(viewMat);
}
