#include "Collision.h"
#include <algorithm>
#include <cmath>

#include"../../Utility/Utility.h"
#include"CollisionUtility.h"

std::vector<UnitBase*> Collision::objects_ = {};
std::vector<UnitBase*> Collision::stageObject_ = {};

Collision::Collision() {}
Collision::~Collision() {}


void Collision::ResolveDynamics(void)
{
    const float EPS = 1e-4f;
    const float cosSlope = std::cos(slopeLimitDeg_ * (DX_PI_F / 180.0f));

    for (auto* o : objects_) {
        if (!o) continue;
        const Base& du = o->GetUnit();

        // カプセル前提（必要なら分岐追加）
        if (du.para_.colliShape != CollisionShape::CAPSULE) continue;

        // 中心と基本寸法
        VECTOR pos = du.pos_;
        VECTOR center = VAdd(du.pos_, du.para_.center);
        VECTOR vel = du.vel_;             // 今フレーム分の移動“相当”
        const float R = du.para_.radius;
        const float H = du.para_.capsuleHalfLen;

        bool grounded = false;

        // 反復解決（複数ヒットの安定化）
        for (int it = 0; it < resolveIters_; ++it) {
            bool moved = false;

            for (auto* s : stageObject_) {
                if (!s) continue;
                const Base& su = s->GetUnit();

                // 1) VoxelBase：セルベースに丸投げ
                if (auto* vox = dynamic_cast<VoxelBase*>(s)) {
                    // 1回ずつ適用（反復外側で複数回）
                    VECTOR center = VAdd(du.pos_, du.para_.center);
                    VECTOR vtmp = vel; bool g = false;
                    if (vox->ResolveCapsuleCenter(center, R, H, vtmp, g, slopeLimitDeg_, 1)) {
                        VECTOR delta = VSub(center, VAdd(du.pos_, du.para_.center));
                        pos = VAdd(pos, delta);
                        VECTOR n = Utility::Normalize(delta);
                        float vn = VDot(vel, n);
                        if (vn < 0.0f) vel = VSub(vel, VScale(n, vn));
                        grounded = grounded || g;
                    }
                    continue;
                }

                // 非Voxel（AABB）：MTV で最小押し戻し + スライド
                if (su.para_.colliShape == CollisionShape::AABB) {
                    VECTOR half = VScale(su.para_.size, 0.5f);
                    VECTOR bmin = VSub(VAdd(su.pos_, su.para_.center), half);
                    VECTOR bmax = VAdd(VAdd(su.pos_, su.para_.center), half);

                    VECTOR n; float d;
                    if (Cfunc::CapsuleAabbY_MTV(center, H, R, bmin, bmax, n, d)) {
                        center = VAdd(center, VScale(n, d + EPS));
                        pos = VAdd(pos, VScale(n, d + EPS));

                        // スライド：法線成分を殺す
                        float vn = VDot(vel, n);
                        if (vn < 0.0f) vel = VSub(vel, VScale(n, vn));

                        // 接地判定（床っぽい法線で下降中なら接地）
                        if (n.y > cosSlope && vel.y <= 0.0f) grounded = true;

                        moved = true;
                    }
                }
            }

            if (!moved) break;
        }

        // 反映
        o->SetPos(pos);
        o->SetVelocity(vel);
        if (grounded) o->OnGrounded();
    }
}

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

	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::AABB) return CapsuleAabb(A, B);
	if (sA == CollisionShape::AABB && sB == CollisionShape::CAPSULE)   return CapsuleAabb(B, A);

	if (sA == CollisionShape::SPHERE && sB == CollisionShape::AABB) return SphereAabb(A, B);
	if (sA == CollisionShape::AABB && sB == CollisionShape::SPHERE)   return SphereAabb(B, A);

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

bool Collision::CapsuleAabb(const Base& capsule, const Base& obb) const
{
	return Cfunc::CapsuleAabb_Y(capsule.pos_, capsule.para_.capsuleHalfLen, capsule.para_.radius, obb.pos_, obb.para_.size);
}

bool Collision::SphereAabb(const Base& sphere, const Base& aabb) const
{
	return Cfunc::SphereAabb(sphere.pos_, sphere.para_.radius, aabb.pos_, aabb.para_.size);
}
