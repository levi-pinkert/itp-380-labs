#include "MeshComponent.h"
#include "SegmentCast.h"
#include "Math.h"
#include <vector>
class Actor;
class Shader;

class LaserComponent : public MeshComponent {
public:
	LaserComponent(Actor* owner);
	virtual void Update(float deltaTime);
	virtual void Draw(class Shader* shader);
protected:
	Matrix4 GetLaserTransform(LineSegment line);
private:
	const float LENGTH = 500.0f;
	std::vector<LineSegment> mLines;

};