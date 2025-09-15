#include "Collision.h"
#include <algorithm>
#include <cmath>

Collision::Collision() : objects_() {}
Collision::~Collision() {}

void Collision::Check()
{
	for (size_t i = 0; i < objects_.size(); ++i) {
		for (size_t j = i + 1; j < objects_.size(); ++j) {
			UnitBase* a = objects_[i];
			UnitBase* b = objects_[j];

			const Base& ua = a->GetUnit();
			const Base& ub = b->GetUnit();

			if (ua.para_.colliType == ub.para_.colliType) continue; // 同陣営スキップ

			if ((ua.aliveCollision_ && !ua.isAlive_) || (ub.aliveCollision_ && !ub.isAlive_)) continue;
			if ((ua.isInvici_ && ua.inviciCounter_ > 0) || (ub.isInvici_ && ub.inviciCounter_ > 0)) continue;

			if (IsHit(ua, ub)) {
				a->OnCollision(b);
				b->OnCollision(a);
			}
		}
	}
}

bool Collision::IsHit(const Base& a, const Base& b)
{
	Base A = a; A.pos_ = VAdd(A.pos_, A.para_.center);
	Base B = b; B.pos_ = VAdd(B.pos_, B.para_.center);

	auto sA = A.para_.colliShape;
	auto sB = B.para_.colliShape;
	if (sA == CollisionShape::NON || sB == CollisionShape::NON) return false;

	// 同種
	if (sA == CollisionShape::SPHERE && sB == CollisionShape::SPHERE)    return SphereSphere(A, B);
	if (sA == CollisionShape::AABB && sB == CollisionShape::AABB) return AabbAabb(A, B);
	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::CAPSULE)   return CapsuleCapsule(A, B);

	// 混合
	if (sA == CollisionShape::SPHERE && sB == CollisionShape::AABB) return SphereAabb(A, B);
	if (sA == CollisionShape::AABB && sB == CollisionShape::SPHERE)    return SphereAabb(B, A);

	if (sA == CollisionShape::SPHERE && sB == CollisionShape::CAPSULE)   return SphereCapsule(A, B);
	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::SPHERE)    return SphereCapsule(B, A);

	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::AABB) return CapsuleAabb(A, B);
	if (sA == CollisionShape::AABB && sB == CollisionShape::CAPSULE)   return CapsuleAabb(B, A);

	return false;
}

Collision::Aabb Collision::MakeAabb(const Base& box) const
{
	Collision::Aabb r;
	r.center = box.pos_;
	r.half = { box.para_.size.x * 0.5f, box.para_.size.y * 0.5f, box.para_.size.z * 0.5f };
	return r;
}

bool Collision::SphereSphere(const Base& a, const Base& b) const
{
	VECTOR d = VSub(a.pos_, b.pos_);
	float rr = (a.para_.radius + b.para_.radius);
	return VDot(d, d) <= rr * rr;
}

bool Collision::AabbAabb(const Base& a, const Base& b) const
{
	Aabb A = MakeAabb(a);
	Aabb B = MakeAabb(b);

	if (A.center.x - A.half.x >= B.center.x + B.half.x) return false;
	if (A.center.x + A.half.x <= B.center.x - B.half.x) return false;
	if (A.center.y - A.half.y >= B.center.y + B.half.y) return false;
	if (A.center.y + A.half.y <= B.center.y - B.half.y) return false;
	if (A.center.z - A.half.z >= B.center.z + B.half.z) return false;
	if (A.center.z + A.half.z <= B.center.z - B.half.z) return false;
	return true;
}

bool Collision::CapsuleCapsule(const Base& a, const Base& b) const 
{
	// 線分-線分最近距離計算（短縮版）
	// 両カプセル中心線の端点
	VECTOR a1 = VAdd(a.pos_, { 0, a.para_.capsuleHalfLen, 0 });
	VECTOR a2 = VSub(a.pos_, { 0, a.para_.capsuleHalfLen, 0 });
	VECTOR b1 = VAdd(b.pos_, { 0, b.para_.capsuleHalfLen, 0 });
	VECTOR b2 = VSub(b.pos_, { 0, b.para_.capsuleHalfLen, 0 });

	// 線分間距離?を求める（必要に応じて完全版を実装）
	// ここでは簡易的に各端点と相手線分距離の最小で判定
	auto pointSegDistSq = [](const VECTOR& p, const VECTOR& s1, const VECTOR& s2) {
		VECTOR seg = VSub(s2, s1);
		VECTOR sp = VSub(p, s1);
		float t = VDot(sp, seg) / VDot(seg, seg);
		t = (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t);
		VECTOR closest = VAdd(s1, VScale(seg, t));
		VECTOR d = VSub(p, closest);
		return VDot(d, d);
		};

	float distSq = (std::min)({
		pointSegDistSq(a1, b1, b2),
		pointSegDistSq(a2, b1, b2),
		pointSegDistSq(b1, a1, a2),
		pointSegDistSq(b2, a1, a2)
		});

	float rr = a.para_.radius + b.para_.radius;
	return distSq <= rr * rr;
}

bool Collision::SphereAabb(const Base& sphere, const Base& aabb) const
{
	Aabb B = MakeAabb(aabb);
	VECTOR minB = { B.center.x - B.half.x, B.center.y - B.half.y, B.center.z - B.half.z };
	VECTOR maxB = { B.center.x + B.half.x, B.center.y + B.half.y, B.center.z + B.half.z };
	VECTOR closest = Utility::Clamp(sphere.pos_, minB, maxB);
	VECTOR d = VSub(sphere.pos_, closest);
	return VDot(d, d) <= sphere.para_.radius * sphere.para_.radius;
}

bool Collision::SphereCapsule(const Base& sphere, const Base& capsule) const {
	// カプセル中心線の端点計算
	VECTOR dir = { 0, capsule.para_.capsuleHalfLen, 0 }; // Y軸方向と仮定（必要に応じ回転対応）
	VECTOR p1 = VAdd(capsule.pos_, dir);
	VECTOR p2 = VSub(capsule.pos_, dir);

	// 球中心から線分最近点
	VECTOR seg = VSub(p2, p1);
	VECTOR sp1 = VSub(sphere.pos_, p1);
	float t = VDot(sp1, seg) / VDot(seg, seg);
	t = (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t);
	VECTOR closest = VAdd(p1, VScale(seg, t));

	VECTOR d = VSub(sphere.pos_, closest);
	float rr = sphere.para_.radius + capsule.para_.radius;
	return VDot(d, d) <= rr * rr;
}

bool Collision::CapsuleAabb(const Base& capsule, const Base& aabb) const {
	// カプセル中心線の端点
	VECTOR p1 = VAdd(capsule.pos_, { 0, capsule.para_.capsuleHalfLen, 0 });
	VECTOR p2 = VSub(capsule.pos_, { 0, capsule.para_.capsuleHalfLen, 0 });

	// AABB最近点との距離比較（線分-ボックス判定）
	// 簡易版: 線分を細かくサンプリングして距離判定（高速化可）
	const int steps = 4;
	for (int i = 0; i <= steps; ++i) {
		float t = (float)i / steps;
		VECTOR p = VAdd(p1, VScale(VSub(p2, p1), t));
		Aabb box = MakeAabb(aabb);
		VECTOR minB = { box.center.x - box.half.x, box.center.y - box.half.y, box.center.z - box.half.z };
		VECTOR maxB = { box.center.x + box.half.x, box.center.y + box.half.y, box.center.z + box.half.z };
		VECTOR closest = Utility::Clamp(p, minB, maxB);
		VECTOR d = VSub(p, closest);
		if (VDot(d, d) <= capsule.para_.radius * capsule.para_.radius) return true;
	}
	return false;
}