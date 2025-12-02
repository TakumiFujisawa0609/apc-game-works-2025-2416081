#pragma once

#include"ColliderBase.h"

class CapsuleCollider : public ColliderBase
{
public:
	CapsuleCollider(TAG type,float halfLen,float radius, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }):
		ColliderBase(type, enoughDistance, pos),
		halfLen_(halfLen),
		radius_(radius)
	{
	}
	~CapsuleCollider()override {}

	float GetHalfLen(void)const { return halfLen_; }
	float GetRadius(void)const { return radius_; }

	// カプセル線分の始点
	Vector3 GetStartPos(void)const {
		// 無回転で真上方向にのばした場所を始点とする
		return GetPos() + (GetTransform().VTrans(Vector3(0.0f, 1.0f, 0.0f)) * halfLen_);
	}
	// カプセル線分の終点
	Vector3 GetEndPos(void)const {
		// 無回転で真下方向にのばした場所を始点とする
		return GetPos() + (GetTransform().VTrans(Vector3(0.0f, -1.0f, 0.0f)) * halfLen_);
	}

private:
	float halfLen_;
	float radius_;
};