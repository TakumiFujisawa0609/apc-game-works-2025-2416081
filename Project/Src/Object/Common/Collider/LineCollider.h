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
	}
	~LineCollider()override {}

	// 線分の長さ
	float GetLen(void)const { return (startPos_ - endPos_).Length(); }

	// 線分の半分の長さ
	float GetHalfLen(void)const { return (startPos_ - endPos_).Length() / 2; }

	// 線分の始点
	Vector3 GetStartPos(void)const { return GetTransform().VTrans(startPos_); }
	// 線分の終点
	Vector3 GetEndPos(void)const { return GetTransform().VTrans(endPos_); }

private:
	// 線分の開始点
	Vector3 startPos_;
	// 線分の終了点
	Vector3 endPos_;
};