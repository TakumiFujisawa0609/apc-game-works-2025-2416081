#pragma once
#include"ColliderBase.h"

class LineCollider : public ColliderBase
{
public:
	/// <summary>
	/// コンストラクト
	/// </summary>
	/// <param name="type">当たり判定タイプ</param>
	/// <param name="halfLength">線分の半分の長さ</param>
	/// <param name="pos">相対座標（引数省略で{0.0f,0.0f,0.0f}）</param>
	LineCollider(TAG type, float halfLength, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		halfLength_(halfLength)
	{
	}
	~LineCollider()override {}

	float GetHalfLength(void)const { return halfLength_; }
	// 線分の始点
	Vector3 GetStartPos(void)const {
		// 無回転で真上方向にのばした場所を始点とする
		return GetPos() + (GetTransform().VTrans(Vector3(0.0f, 1.0f, 0.0f)) * halfLength_);
	}
	// 線分の終点
	Vector3 GetEndPos(void)const {
		// 無回転で真下方向にのばした場所を始点とする
		return GetPos() + (GetTransform().VTrans(Vector3(0.0f, -1.0f, 0.0f)) * halfLength_);
	}
private:
	float halfLength_;
};