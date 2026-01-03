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
		SetShape(SHAPE::CAPSULE);
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
	Vector3 GetStartPos(void)const { return GetPos() + GetTransform().VTrans(startPos_); }
	// カプセル線分の終点
	Vector3 GetEndPos(void)const { return GetPos() + GetTransform().VTrans(endPos_); }

	// 指定した座標から線分の中で一番近い座標を取得する
	Vector3 ClosestPoint(const Vector3& point) const {
		// 始点と終点
		Vector3 s = GetStartPos();
		Vector3 e = GetEndPos();

		Vector3 se = e - s;
		Vector3 sp = point - s;

		float lenSq = se.LengthSq();
		if (lenSq < 1e-6f) {
			// 始点 = 終点 の場合、始点を返す
			return s;
		}

		// 点を線分に射影するパラメータt
		float t = sp.Dot(se) / lenSq;

		// 線分範囲に clamp
		t = std::clamp(t, 0.0f, 1.0f);

		// 最近点
		return s + se * t;
	}
#pragma endregion

	void DrawDebug(unsigned int color = 0xffffff)override {
		DrawCapsule3D(GetStartPos().ToVECTOR(), GetEndPos().ToVECTOR(), radius_, 12, color, color, true);
	}
private:

	// 線分の開始点
	Vector3 startPos_;
	// 線分の終了点
	Vector3 endPos_;
	
	// 半径
	float radius_;
};