#include "UnitBase.h"

#include <DxLib.h>

#include"../Scene/SceneManager/SceneManager.h"

UnitBase::UnitBase() :yAccelSum_(0.0f) {}
UnitBase::~UnitBase() {}

void UnitBase::Invi(void)
{
	if (unit_.inviciCounter_ > 0) { unit_.inviciCounter_--; }
}

void UnitBase::AccelUpdate(void)
{
	unit_.pos_.y += yAccelSum_;
}

void UnitBase::DrawDebug(void)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	switch (unit_.para_.colliShape)
	{
	case CollisionShape::NON:break;
	case CollisionShape::SPHERE:
		DrawSphere3D(unit_.WorldPos(), unit_.para_.radius, 12, 0xffffff, 0xffffff, true);
		break;
	case CollisionShape::OBB:
	case CollisionShape::AABB:
		
		break;
	case CollisionShape::CAPSULE:
		VECTOR localPos = { 0.0f,unit_.para_.capsuleHalfLen,0.0f };
		DrawCapsule3D(
			VSub(unit_.WorldPos(), localPos),
			VAdd(unit_.WorldPos(), localPos),
			unit_.para_.radius, 12, 0xffffff, 0xffffff, true);
		break;
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


void UnitBase::Gravity(void)
{
	yAccelSum_ += GRAVITY;
	if (yAccelSum_ < GRAVITY_MAX) {
		yAccelSum_ = GRAVITY_MAX; 
	}
}
