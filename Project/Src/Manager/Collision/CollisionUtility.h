#pragma once

#include"../../Utility/Utility.h"

class CollisionUtility {
public:
    static bool Sphere
    (
        const VECTOR& pos1, float radius1,
		const VECTOR& pos2, float radius2
    );

    static bool Obb(
        const VECTOR& pos1, const VECTOR& size1, const VECTOR& angle1,
        const VECTOR& pos2, const VECTOR& size2, const VECTOR& angle2
    );

    static bool Capsule(
        const VECTOR& pos1, float halfLen1, float r1, const VECTOR& ang1,
        const VECTOR& pos2, float halfLen2, float r2, const VECTOR& ang2
    );

    static bool SphereObb(
        const VECTOR& spherePos, float sphereRadius,
        const VECTOR& obbPos, const VECTOR& obbSize, const VECTOR& obbAngle
    );

    static bool SphereCapsule(
        const VECTOR& spherePos, float sphereRadius,
        const VECTOR& capsulePos, float capsuleHalfLen, float capsuleRadius, const VECTOR& capsuleAngle
    );

    static bool CapsuleObb(
        const VECTOR& capsulePos, float capsuleHalfLen, float capsuleRadius, const VECTOR& capsuleAngle,
        const VECTOR& obbPos, const VECTOR& obbSize, const VECTOR& obbAngle
    );

    static bool CapsuleAabb_Y(const VECTOR& capsuleCenter, float capsuleHalfLen, float capsuleRadius,
        const VECTOR& boxCenter, const VECTOR& boxSize);

private:
    // ---- 内部ヘルパ ----
    static inline float comp(const VECTOR& v, int i) { return i == 0 ? v.x : (i == 1 ? v.y : v.z); }

    struct ObbInfo {
        VECTOR pos;
        VECTOR half;     // 半サイズ
        VECTOR axis[3];  // ワールド基底（正規直交）
    };

    static ObbInfo  MakeObb(const VECTOR& pos, const VECTOR& size, const VECTOR& angle);
    static float PointSegmentDistSq(const VECTOR& p, const VECTOR& a, const VECTOR& b);
    static float SegmentSegmentDistSq(const VECTOR& p1, const VECTOR& q1, const VECTOR& p2, const VECTOR& q2);
    static float SegmentAabbDistSq_Local(const VECTOR& p0, const VECTOR& p1, const VECTOR& half, int steps = 12);
};

using Cfunc = CollisionUtility;