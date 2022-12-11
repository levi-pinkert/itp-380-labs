#include "CollisionComponent.h"
#include "Actor.h"

CollisionComponent::CollisionComponent(class Actor* owner)
:Component(owner)
,mWidth(0.0f)
,mHeight(0.0f)
,mDepth(0.0f)
{
	
}

CollisionComponent::~CollisionComponent()
{
	
}

bool CollisionComponent::Intersect(const CollisionComponent* other)
{
	Vector3 aMin = GetMin();
	Vector3 aMax = GetMax();
	Vector3 bMin = other->GetMin();
	Vector3 bMax = other->GetMax();

	bool no = aMax.x < bMin.x ||
		aMax.y < bMin.y ||
		aMax.z < bMin.z ||
		bMax.x < aMin.x ||
		bMax.y < aMin.y ||
		bMax.z < aMin.z;

	return !no;
}

Vector3 CollisionComponent::GetMin() const
{
	Vector3 v = mOwner->GetPosition();
	v.x -= mDepth * mOwner->GetScale() / 2.0f;
	v.y -= mWidth * mOwner->GetScale() / 2.0f;
	v.z -= mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetMax() const
{
	Vector3 v = mOwner->GetPosition();
	v.x += mDepth * mOwner->GetScale() / 2.0f;
	v.y += mWidth * mOwner->GetScale() / 2.0f;
	v.z += mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

const Vector3& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(
	const CollisionComponent* other, Vector3& offset)
{
	//Check if there is an intersection at all
	offset = Vector3::Zero;
	if (!Intersect(other)) {
		return CollSide::None;
	}

	//Find (signed) distances
	Vector3 aMin = GetMin(), aMax = GetMax(), bMin = other->GetMin(), bMax = other->GetMax();
	float dists[] = {
		bMax.x - aMin.x, //frontDist
		bMin.x - aMax.x, //backDist
		bMax.y - aMin.y, //rightDist
		bMin.y - aMax.y, //leftDist
		bMax.z - aMin.z, //topDist
		bMin.z - aMax.z //bottomDist
	};

	//Find direction with minimum absolute value
	size_t min = 0;
	for (size_t i = 1; i < 6; i++) {
		if (Math::Abs(dists[i]) < Math::Abs(dists[min])) { min = i; }
	}

	switch (min) {
	case(0):
		offset.x += dists[0];
		return CollSide::Front;
	case(1):
		offset.x += dists[1];
		return CollSide::Back;
	case(2):
		offset.y += dists[2];
		return CollSide::Right;
	case(3):
		offset.y += dists[3];
		return CollSide::Left;
	case(4):
		offset.z += dists[4];
		return CollSide::Top;
	case(5):
		offset.z += dists[5];
		return CollSide::Bottom;
	}
	return CollSide::None;
}
