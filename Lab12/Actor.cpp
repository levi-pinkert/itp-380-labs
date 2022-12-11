#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game, Actor* parent)
	:mGame(game)
	,mState(ActorState::Active)
	,mPosition(Vector3::Zero)
	,mScale(1.0f)
	,mRotation(0.0f)
	,mRoll(0.0f)
	,mParent(parent)
	,mInheritScale(false)
{
	if (mParent == nullptr) {
		mGame->AddActor(this);
	}
	else {
		mParent->AddChild(this);
	}
}

Actor::~Actor()
{
	//Delete all children
	while (!mChildren.empty()) {
		delete mChildren.back();	//they remove themselves from mChildren
	}

	//Remove this from parent/game
	if (mParent == nullptr) {
		mGame->RemoveActor(this);
	}
	else {
		mParent->RemoveChild(this);
	}

	//Delete components
	for (size_t i = 0; i < mComponents.size(); i++) {
		delete mComponents[i];
	}
	mComponents.clear();
}

void Actor::Update(float deltaTime)
{
	//Calculate world transform
	CalcWorldTransform();

	if (mState == ActorState::Active) {
		//Update all components
		for (size_t i = 0; i < mComponents.size(); i++) {
			mComponents[i]->Update(deltaTime);
		}
		//Run Actor's Update Code
		OnUpdate(deltaTime);
	}

	//Update children
	CalcWorldTransform();
	for (Actor* child : mChildren) {
		child->Update(deltaTime);
	}
}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState)
{
	if (mState == ActorState::Active) {
		//Let all components process input
		for (size_t i = 0; i < mComponents.size(); i++) {
			mComponents[i]->ProcessInput(keyState);
		}
		//Actor's Input Processing
		OnProcessInput(keyState);
	}
}

void Actor::OnProcessInput(const Uint8* keyState)
{
}

void Actor::AddComponent(Component* c)
{
	mComponents.emplace_back(c);
	std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
		return a->GetUpdateOrder() < b->GetUpdateOrder();
	});
}

Vector3 Actor::GetForward() const {
	return Vector3(Math::Cos(mRotation), Math::Sin(mRotation), 0.0f);
}

Vector3 Actor::GetRight() const {
	return Vector3(Math::Cos(mRotation + Math::PiOver2), Math::Sin(mRotation + Math::PiOver2), 0.0f);
}

Vector3 Actor::GetQuatForward() {
	Vector3 fwd = Vector3::Transform(Vector3::UnitX, mQuaternion);
	fwd.Normalize();
	return fwd;
}

void Actor::CalcWorldTransform() {
	mWorldTransform = Matrix4::CreateScale(mScale)
		* Matrix4::CreateRotationZ(mRotation)
		* Matrix4::CreateRotationX(mRoll)
		* Matrix4::CreateFromQuaternion(mQuaternion)
		* Matrix4::CreateTranslation(mPosition);
	if (mParent != nullptr) {
		if (mInheritScale) {
			mWorldTransform *= mParent->GetWorldTransform();
		}
		else {
			mWorldTransform *= mParent->GetWorldRotTrans();
		}
	}
}

Matrix4 Actor::GetWorldRotTrans() {
	return Matrix4::CreateRotationZ(mRotation)
		* Matrix4::CreateRotationX(mRoll)
		* Matrix4::CreateFromQuaternion(mQuaternion)
		* Matrix4::CreateTranslation(mPosition);
}

Vector3 Actor::GetWorldPosition() {
	return mWorldTransform.GetTranslation();
}

Vector3 Actor::GetWorldForward() {
	return mWorldTransform.GetXAxis();
}

void Actor::AddChild(Actor* child) {
	mChildren.push_back(child);
}

void Actor::RemoveChild(Actor* child) {
	//Search for the child
	auto it = mChildren.begin();
	while (it != mChildren.end() && *it != child) {
		++it;
	}

	//Remove it if it exists
	if (it != mChildren.end()) {
		mChildren.erase(it);
	}
}