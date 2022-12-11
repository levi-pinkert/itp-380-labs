#include "CameraComponent.h"
#include "Math.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

CameraComponent::CameraComponent(Actor* owner) : Component(owner) {

}

void CameraComponent::Update(float deltaTime) {
	//Spring towards ideal position
	Vector3 idealPos = CalculateIdeal();
	Vector3 displacement = position - idealPos;
	Vector3 accel = (-SPRING * displacement) - (DAMPENING * velocity);
	velocity += accel * deltaTime;
	position += velocity * deltaTime;

	//Update the view matrix
	UpdateViewMatrix();
}

Vector3 CameraComponent::CalculateIdeal() {
	Vector3 ownerPos = mOwner->GetPosition();
	Vector3 ownerFwd = mOwner->GetForward();
	Vector3 idealPos = ownerPos - (ownerFwd * H_DIST);
	idealPos.z = Z_POS;
	return idealPos;
}

void CameraComponent::SnapToIdeal() {
	position = CalculateIdeal();
	UpdateViewMatrix();
}

void CameraComponent::UpdateViewMatrix() {
	//Set view matrix
	Vector3 ownerPos = mOwner->GetPosition();
	Vector3 ownerFwd = mOwner->GetForward();
	Vector3 targetPos = ownerPos + (ownerFwd * TARGET_DIST);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(Matrix4::CreateLookAt(position, targetPos, Vector3::UnitZ));
}