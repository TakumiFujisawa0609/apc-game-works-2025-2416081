#pragma once
#include"ColliderBase.h"

class LineCollider : public ColliderBase
{
public:
	/// <summary>
	/// コンストラクト
	/// </summary>
	/// <param name="type">当たり判定タイプ</param>
	/// <param name="localStartPos">線分の開始点</param>
	/// <param name="localEndPos">線分の終了点</param>
	/// <param name="enoughDistance">判定スキップに十分な距離　-1.0fで未設定とし、距離による判定スキップを行わない（引数省略で-1.0f）</param>
	/// <param name="pos">相対座標（引数省略で{0.0f,0.0f,0.0f}）</param>
	LineCollider(TAG type, const Vector3& localStartPos, const Vector3& localEndPos, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		startPos_(localStartPos),
		endPos_(localEndPos)
	{
		SetShape(SHAPE::LINE);
	}
	~LineCollider()override {}

	// 線分の長さ
	float GetLen(void)const { return (startPos_ - endPos_).Length(); }

	// 線分の半分の長さ
	float GetHalfLen(void)const { return (startPos_ - endPos_).Length() / 2; }

	// 線分の始点
	Vector3 GetStartPos(void)const { return GetPos() + GetTransform().VTrans(startPos_); }
	// 線分の終点
	Vector3 GetEndPos(void)const { return GetPos() + GetTransform().VTrans(endPos_); }

	// 押し出しの方向
	Vector3 GetDirection(void)const { return GetStartPos() - GetEndPos(); }

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

	Vector3 ClosestPointAABB(const Vector3& bmin, const Vector3& bmax) const {
		// AABB 内部の基準点として clamp する
		Vector3 p = ClosestPoint(
			Vector3(
				std::clamp(GetPos().x, bmin.x, bmax.x),
				std::clamp(GetPos().y, bmin.y, bmax.y),
				std::clamp(GetPos().z, bmin.z, bmax.z)
			)
		);

		// さらに AABB 側の clamp
		return {
			std::clamp(p.x, bmin.x, bmax.x),
			std::clamp(p.y, bmin.y, bmax.y),
			std::clamp(p.z, bmin.z, bmax.z)
		};
	}

private:
	// 線分の開始点
	Vector3 startPos_;
	// 線分の終了点
	Vector3 endPos_;
};