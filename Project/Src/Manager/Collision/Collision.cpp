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
    const float EPS2 = EPS * EPS;
    const float POS_SLOP = 5e-4f;                 // 微小貫通は無視
    const float cosSlope = std::cos(slopeLimitDeg_ * (DX_PI_F / 180.0f));

    for (auto* o : objects_) {
        if (!o) continue;
        const Base& du = o->GetUnit();
        if (du.para_.colliShape != CollisionShape::CAPSULE) continue;

        VECTOR pos = du.pos_;
        VECTOR vel = du.vel_;
        const float R = du.para_.radius;
        const float H = du.para_.capsuleHalfLen;

        bool grounded = false;

        // 反復解決
        VECTOR prevN = { 0,0,0 }; // 反復内の法線スムージング用
        for (int it = 0; it < resolveIters_; ++it) {

            VECTOR center = VAdd(pos, du.para_.center);

            // その反復での “最良” 接触を一つ選ぶ
            bool   found = false;
            float  bestW = 0.0f;       // 重み（AABB=深さ, Voxel=押し戻し長さ）
            VECTOR bestPush = { 0,0,0 }; // 実際に加える押し戻し
            VECTOR bestN = { 0,0,0 }; // 対応する法線
            bool   bestGnd = false;

            for (auto* s : stageObject_) {
                if (!s) continue;
                const Base& su = s->GetUnit();

                // ---- Voxel：中心版で1回だけ解く → delta を候補に ----
                if (auto* vox = dynamic_cast<VoxelBase*>(s)) {
                    VECTOR preC = center;
                    VECTOR vtmp = vel; bool g = false;
                    if (vox->ResolveCapsuleCenter(center, R, H, vtmp, g, slopeLimitDeg_, 1)) {
                        VECTOR delta = VSub(center, preC);
                        float  d2 = VDot(delta, delta);
                        if (d2 > bestW && d2 > EPS2) {
                            bestW = d2;                // Voxel は押し戻し長さで比較
                            bestPush = delta;
                            // 法線は押し戻し方向
                            float L = std::sqrt(d2);
                            bestN = { delta.x / L, delta.y / L, delta.z / L };
                            bestGnd = g;
                            found = true;
                        }
                    }
                    // center は local のみで使うので戻す
                    center = preC;
                    continue;
                }

                // ---- 非Voxel（AABB）：MTVを候補に ----
                if (su.para_.colliShape == CollisionShape::AABB) {
                    const VECTOR half = VScale(su.para_.size, 0.5f);
                    const VECTOR bmin = VSub(VAdd(su.pos_, su.para_.center), half);
                    const VECTOR bmax = VAdd(VAdd(su.pos_, su.para_.center), half);

                    VECTOR n; float d;
                    if (Cfunc::CapsuleAabbY_MTV(center, H, R, bmin, bmax, n, d)) {
                        if (d > bestW && d > POS_SLOP) {   // AABB は“深さ”で比較
                            bestW = d;
                            bestPush = VScale(n, d + EPS);
                            bestN = n;
                            // 床っぽい方向なら接地候補
                            bestGnd = (n.y > cosSlope && vel.y <= 0.0f);
                            found = true;
                        }
                    }
                }
            } // for stage

            if (!found) break; // もう押し戻すべき接触なし

            // 法線スムージング（反復内ヒステリシス）
            if (prevN.x != 0 || prevN.y != 0 || prevN.z != 0) {
                float dp = prevN.x * bestN.x + prevN.y * bestN.y + prevN.z * bestN.z;
                if (dp > 0.0f) { // 反転時はそのまま
                    const float a = 0.6f; // ブレンド係数（上げるとより粘る）
                    VECTOR blended = { bestN.x * a + prevN.x * (1 - a),
                                       bestN.y * a + prevN.y * (1 - a),
                                       bestN.z * a + prevN.z * (1 - a) };
                    float L2 = blended.x * blended.x + blended.y * blended.y + blended.z * blended.z;
                    if (L2 > 1e-12f) {
                        float inv = 1.0f / std::sqrt(L2);
                        bestN = { blended.x * inv, blended.y * inv, blended.z * inv };
                    }
                }
            }
            prevN = bestN;

            // 一回だけ押し戻す
            pos = VAdd(pos, bestPush);
            center = VAdd(center, bestPush);

            // スライド（法線成分をカット）
            float vn = VDot(vel, bestN);
            if (vn < 0.0f) vel = VSub(vel, VScale(bestN, vn));

            grounded = grounded || bestGnd;
        } // for it

        // 微小速度の丸め（角での小刻みな振動対策）
        auto killTiny = [](float& c) { if (std::fabs(c) < 1e-6f) c = 0.0f; };
        killTiny(vel.x); killTiny(vel.y); killTiny(vel.z);

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
