#include "CollisionComponent.h"
#include "Actor.h"

CollisionComponent::CollisionComponent(class Actor* owner)
:Component(owner)
,mWidth(0.0f)
,mHeight(0.0f)
{
	
}

CollisionComponent::~CollisionComponent()
{
	
}

bool CollisionComponent::Intersect(const CollisionComponent* other)
{
	Vector2 aMin = GetMin(), aMax = GetMax(), bMin = other->GetMin(), bMax = other->GetMax();
	return !(aMax.x < bMin.x || aMax.y < bMin.y || aMin.x > bMax.x || aMin.y > bMax.y);
}

Vector2 CollisionComponent::GetMin() const
{
	return mOwner->GetPosition() - (Vector2(mWidth, mHeight) * mOwner->GetScale() * 0.5f);
}

Vector2 CollisionComponent::GetMax() const
{
	return mOwner->GetPosition() + (Vector2(mWidth, mHeight) * mOwner->GetScale() * 0.5f);
}

const Vector2& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(
	const CollisionComponent* other, Vector2& offset)
{
	//Check if there is an intersection at all
	offset = Vector2::Zero;
	if (!Intersect(other)) {
		return CollSide::None;
	}

	//Find (signed) distances
	Vector2 aMin = GetMin(), aMax = GetMax(), bMin = other->GetMin(), bMax = other->GetMax();
	float dists[] = {
		bMax.x - aMin.x, //rightDist
		bMin.y - aMax.y, //topDist
		bMin.x - aMax.x, //leftDist
		bMax.y - aMin.y //bottomDist
	};

	//Find direction with minimum absolute value
	size_t min = 0;
	for (size_t i = 1; i < 4; i++) {
		if (Math::Abs(dists[i]) < Math::Abs(dists[min])) { min = i; }
	}
	switch (min) {
	case(0):
		offset.x += dists[0];
		return CollSide::Right;
	case(1):
		offset.y += dists[1];
		return CollSide::Top;
	case(2):
		offset.x += dists[2];
		return CollSide::Left;
	case(3):
		offset.y += dists[3];
		return CollSide::Bottom;
	}
	return CollSide::None;
}
