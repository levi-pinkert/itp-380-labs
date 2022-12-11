#include "LaserComponent.h"
#include "Actor.h"
#include "Shader.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Math.h"
#include "SegmentCast.h"
#include <vector>
#include "Block.h"
#include "Player.h"

LaserComponent::LaserComponent(Actor* owner) : MeshComponent(owner, true) {
	SetMesh(mOwner->GetGame()->GetRenderer()->GetMesh("Assets/Laser.gpmesh"));
}

void LaserComponent::Update(float deltaTime) {
	//Set the laser's position
	mLines.clear();
	LineSegment firstLine;
	firstLine.mStart = mOwner->GetWorldPosition();
	firstLine.mEnd = firstLine.mStart + LENGTH * mOwner->GetWorldForward();
	mLines.push_back(firstLine);

	//Check collision with player and walls
	CastInfo castRes;
	Actor* ignoreActor = mOwner->GetParent();
	bool reflected = true;
	while (reflected) {
		reflected = false;
		if (SegmentCast((Actor*)mOwner->GetGame()->GetPlayer(), mLines.back(), castRes)) {
			//Hit the player
			mLines.back().mEnd = castRes.mPoint;
			mOwner->GetGame()->GetPlayer()->Die();
			Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Laser.wav"), 0);
		}
		else if (SegmentCast(mOwner->GetGame()->GetBlocks(), mLines.back(), castRes, ignoreActor)) {
			//Hit a block
			mLines.back().mEnd = castRes.mPoint;
			if (((Block*)castRes.mActor)->GetIsMirror()) {
				//Reflect
				LineSegment newLine;
				newLine.mStart = castRes.mPoint;
				Vector3 lastDir = mLines.back().mEnd - mLines.back().mStart;
				lastDir.Normalize();
				Vector3 reflectDir = Vector3::Reflect(lastDir, castRes.mNormal);
				newLine.mEnd = newLine.mStart + reflectDir * LENGTH;
				mLines.push_back(newLine);
				ignoreActor = castRes.mActor;
				reflected = true;
			}
		}
	}
}

void LaserComponent::Draw(class Shader* shader) {
	if (mMesh)
	{
		//Draw each line
		for (LineSegment line : mLines) {
			// Set the world transform
			shader->SetMatrixUniform("uWorldTransform",
				GetLaserTransform(line));
			// Set the active texture
			Texture* t = mMesh->GetTexture(mTextureIndex);
			if (t)
			{
				t->SetActive();
			}
			// Set the mesh's vertex array as active
			VertexArray* va = mMesh->GetVertexArray();
			va->SetActive();
			// Draw
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

Matrix4 LaserComponent::GetLaserTransform(LineSegment line) {
	//Calculate rotation matrix
	Vector3 fwd = line.mEnd - line.mStart;
	fwd.Normalize();
	float rotTheta = Math::Acos(Vector3::Dot(Vector3::UnitX, fwd));
	Vector3 rotAxis = Vector3::Cross(Vector3::UnitX, fwd);
	float rotDot = Vector3::Dot(Vector3::UnitX, fwd);
	Quaternion quat;
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
	
	//Calculate the transform
	Matrix4 result = Matrix4::CreateScale(line.Length(), 1.0f, 1.0f);
	result *= Matrix4::CreateFromQuaternion(quat);
	result *= Matrix4::CreateTranslation(line.PointOnSegment(0.5f));
	return result;
}