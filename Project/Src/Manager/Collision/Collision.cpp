#include "Collision.h"
#include <algorithm>
#include <cmath>

#include"../../Utility/Utility.h"
#include"CollisionUtility.h"

std::vector<UnitBase*> Collision::objects_ = {};
std::vector<UnitBase*> Collision::stageObject_ = {};

Collision::Collision() {}
Collision::~Collision() {}

void Collision::Check()
{
	// ステージオブジェクトとオブジェクトの当たり判定
	for (auto& s : stageObject_) {
		for (auto& o : objects_) {
			const Base& us = s->GetUnit();
			const Base& uo = o->GetUnit();

			if ((us.aliveCollision_ && !us.isAlive_) || (uo.aliveCollision_ && !uo.isAlive_)) continue;

			if (IsHit(us, uo)) {
				s->OnCollision(o);
				o->OnCollision(s);
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
	if (sA == CollisionShape::OBB && sB == CollisionShape::OBB) return ObbObb(A, B);
	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::CAPSULE)   return CapsuleCapsule(A, B);

	// 混合
	if (sA == CollisionShape::SPHERE && sB == CollisionShape::OBB) return SphereObb(A, B);
	if (sA == CollisionShape::OBB && sB == CollisionShape::SPHERE)    return SphereObb(B, A);

	if (sA == CollisionShape::SPHERE && sB == CollisionShape::CAPSULE)   return SphereCapsule(A, B);
	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::SPHERE)    return SphereCapsule(B, A);

	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::OBB) return CapsuleObb(A, B);
	if (sA == CollisionShape::OBB && sB == CollisionShape::CAPSULE)   return CapsuleObb(B, A);

	return false;
}

bool Collision::SphereSphere(const Base& a, const Base& b) const
{
	return Cfunc::Sphere(a.pos_, a.para_.radius, b.pos_, b.para_.radius);
}

bool Collision::ObbObb(const Base& a, const Base& b) const
{
	return Cfunc::Obb(
		a.pos_, a.para_.size, a.angle_,
		b.pos_, b.para_.size, b.angle_
	);
}

bool Collision::CapsuleCapsule(const Base& a, const Base& b) const 
{
	return Cfunc::Capsule(
		a.pos_, a.para_.radius, a.para_.capsuleHalfLen, a.angle_,
		b.pos_, b.para_.radius, b.para_.capsuleHalfLen, b.angle_
	);
}

bool Collision::SphereObb(const Base& sphere, const Base& obb) const
{
	return Cfunc::SphereObb(sphere.pos_, sphere.para_.radius,
		obb.pos_, obb.para_.size, obb.angle_);
}

bool Collision::SphereCapsule(const Base& sphere, const Base& capsule) const 
{
	return Cfunc::SphereCapsule(sphere.pos_, sphere.para_.radius,
		capsule.pos_, capsule.para_.radius, capsule.para_.capsuleHalfLen, capsule.angle_);
}

bool Collision::CapsuleObb(const Base& capsule, const Base& obb) const 
{
	return Cfunc::CapsuleObb(
		capsule.pos_, capsule.para_.radius, capsule.para_.capsuleHalfLen, capsule.angle_,
		obb.pos_, obb.para_.size, obb.angle_
	);
}