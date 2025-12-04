#pragma once

#include"ColliderBase.h"

class CapsuleCollider : public ColliderBase
{
public:
	CapsuleCollider(TAG type, const Vector3& localStartPos, const Vector3& localEndPos, float radius, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		startPos_(localStartPos),
		endPos_(localEndPos),
		radius_(radius)
	{
	}
	~CapsuleCollider()override {}

#pragma region 各ゲット関数
	// 線分の長さ
	float GetLen(void)const { return (startPos_ - endPos_).Length(); }
	// 線分の半分の長さ
	float GetHalfLen(void)const { return (startPos_ - endPos_).Length() / 2; }
	// 半径
	float GetRadius(void)const { return radius_; }

	// カプセル線分の始点
	Vector3 GetStartPos(void)const { return GetTransform().VTrans(startPos_); }
	// カプセル線分の終点
	Vector3 GetEndPos(void)const { return GetTransform().VTrans(endPos_); }
#pragma endregion

private:

	// 線分の開始点
	Vector3 startPos_;
	// 線分の終了点
	Vector3 endPos_;
	
	// 半径
	float radius_;
};