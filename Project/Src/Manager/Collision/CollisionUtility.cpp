#include"CollisionUtility.h"

#include <algorithm>
#include <cmath>

bool CollisionUtility::Sphere(const VECTOR& pos1, float radius1, const VECTOR& pos2, float radius2)
{
	VECTOR d = VSub(pos1, pos2);
	float rr = (radius1 + radius2);
	return VDot(d, d) <= rr * rr;
}

bool CollisionUtility::Obb(const VECTOR& pos1, const VECTOR& size1, const VECTOR& angle1, const VECTOR& pos2, const VECTOR& size2, const VECTOR& angle2)
{
	ObbInfo A = MakeObb(pos1, size1, angle1);
	ObbInfo B = MakeObb(pos2, size2, angle2);

	// 回転行列 R_ij = Ai・Bj
	float R[3][3], AbsR[3][3];
	const float EPS = 1e-6f;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			R[i][j] = VDot(A.axis[i], B.axis[j]);
			AbsR[i][j] = std::fabs(R[i][j]) + EPS;
		}
	}

	// t を A のローカル基底へ
	VECTOR t3 = VSub(B.pos, A.pos);
	float t[3] = { VDot(t3, A.axis[0]), VDot(t3, A.axis[1]), VDot(t3, A.axis[2]) };

	// 1) A の軸
	for (int i = 0; i < 3; ++i) {
		float ra = comp(A.half, i);
		float rb = B.half.x * AbsR[i][0] + B.half.y * AbsR[i][1] + B.half.z * AbsR[i][2];
		if (std::fabs(t[i]) > ra + rb) return false;
	}

	// 2) B の軸
	for (int i = 0; i < 3; ++i) {
		float ra = A.half.x * AbsR[0][i] + A.half.y * AbsR[1][i] + A.half.z * AbsR[2][i];
		float rb = comp(B.half, i);
		float tj = std::fabs(VDot(t3, B.axis[i]));
		if (tj > ra + rb) return false;
	}

	// 3) 交差軸 9本（Ai x Bj）
	// 関数内で安全に各成分を取得
	auto comp = [](const VECTOR& v, int i) -> float {
		if (i == 0) return v.x;
		if (i == 1) return v.y;
		return v.z;
		};

	for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) {
		float ra = A.half.y * AbsR[(i + 2) % 3][j] + A.half.z * AbsR[(i + 1) % 3][j];
		float rb = B.half.y * AbsR[i][(j + 1) % 3] + B.half.z * AbsR[i][(j + 2) % 3];

		// |t・(Ai×Bj)| を計算。Ai×Bj は正規直交系の外積なので、tをB基底へ写して近似
		// 簡易で | t.y * R[(i+2)%3][j] - t.z * R[(i+1)%3][j] | のように成分を組む方法が知られている
		float tval = std::fabs(
			t[(i + 1) % 3] * R[(i + 2) % 3][j] - t[(i + 2) % 3] * R[(i + 1) % 3][j]
		);

		if (tval > ra + rb) return false;
	}

	return true;
}

bool CollisionUtility::Capsule(const VECTOR& pos1, float halfLen1, float r1, const VECTOR& ang1, const VECTOR& pos2, float halfLen2, float r2, const VECTOR& ang2)
{
	// 中心線方向（Y軸を回転）
	VECTOR aDir = VTransform({ 0, 1, 0 }, Utility::MatrixAllMultXYZ({ ang1 }));
	VECTOR bDir = VTransform({ 0, 1, 0 }, Utility::MatrixAllMultXYZ({ ang2 }));

	VECTOR a1 = VAdd(pos1, VScale(aDir, halfLen1));
	VECTOR a2 = VSub(pos1, VScale(aDir, halfLen1));
	VECTOR b1 = VAdd(pos2, VScale(bDir, halfLen2));
	VECTOR b2 = VSub(pos2, VScale(bDir, halfLen2));

	float distSq = SegmentSegmentDistSq(a1, a2, b1, b2);
	float rr = r1 + r2;
	return distSq <= rr * rr;
}

bool CollisionUtility::SphereObb(const VECTOR& spherePos, float sphereRadius, const VECTOR& obbPos, const VECTOR& obbSize, const VECTOR& obbAngle)
{
	ObbInfo obb = MakeObb(obbPos, obbSize, obbAngle);

	// 球中心→OBB ローカルへ
	VECTOR d = VSub(spherePos, obb.pos);
	float local[3] = { VDot(d, obb.axis[0]), VDot(d, obb.axis[1]), VDot(d, obb.axis[2]) };

	// クリップして最近点
	float qx = (local[0] < -obb.half.x) ? -obb.half.x : (local[0] > obb.half.x ? obb.half.x : local[0]);
	float qy = (local[1] < -obb.half.y) ? -obb.half.y : (local[1] > obb.half.y ? obb.half.y : local[1]);
	float qz = (local[2] < -obb.half.z) ? -obb.half.z : (local[2] > obb.half.z ? obb.half.z : local[2]);

	// 最近点をワールドへ戻す
	VECTOR closest = VAdd(obb.pos,
		VAdd(VScale(obb.axis[0], qx),
			VAdd(VScale(obb.axis[1], qy),
				VScale(obb.axis[2], qz))));

	VECTOR diff = VSub(spherePos, closest);
	return VDot(diff, diff) <= sphereRadius * sphereRadius;
}

bool CollisionUtility::SphereCapsule(const VECTOR& spherePos, float sphereRadius, const VECTOR& capsulePos, float capsuleHalfLen, float capsuleRadius, const VECTOR& capsuleAngle)
{
	// カプセルの中心線方向（Y軸ベースを angle_ で回転）
	VECTOR dir = VTransform({ 0.0f, 1.0f, 0.0f }, Utility::MatrixAllMultXYZ({ capsuleAngle }));

	// 中心線の端点（center は IsHit で pos_ に既に加算済み）
	VECTOR p1 = VAdd(capsulePos, VScale(dir, capsuleHalfLen));
	VECTOR p2 = VSub(capsulePos, VScale(dir, capsuleHalfLen));

	// 球中心から中心線（線分）までの最近距離
	float dist2 = PointSegmentDistSq(spherePos, p1, p2);

	// 半径の和と比較（カプセル半径＋球半径）
	float rr = sphereRadius + sphereRadius;
	return dist2 <= rr * rr;
}

bool CollisionUtility::CapsuleObb(const VECTOR& capsulePos, float capsuleHalfLen, float capsuleRadius, const VECTOR& capsuleAngle, const VECTOR& obbPos, const VECTOR& obbSize, const VECTOR& obbAngle)
{
	ObbInfo B = MakeObb(obbPos, obbSize, obbAngle);

	// カプセル中心線（任意向き）
	VECTOR dir = VTransform({ 0, 1, 0 }, Utility::MatrixAllMultXYZ({ capsuleAngle }));
	VECTOR p1 = VAdd(capsulePos, VScale(dir, capsuleHalfLen));
	VECTOR p2 = VSub(capsulePos, VScale(dir, capsuleHalfLen));

	// OBB ローカルへ変換（原点=box.center、基底=axis[3]）
	auto toLocal = [&](const VECTOR& p)->VECTOR {
		VECTOR d = VSub(p, B.pos);
		return {
			VDot(d, B.axis[0]),
			VDot(d, B.axis[1]),
			VDot(d, B.axis[2])
		};
		};

	VECTOR lp1 = toLocal(p1);
	VECTOR lp2 = toLocal(p2);

	float distSq = SegmentAabbDistSq_Local(lp1, lp2, B.half);
	return distSq <= capsuleRadius * capsuleRadius;
}

bool CollisionUtility::CapsuleAabb_Y(const VECTOR& capsuleCenter, float capsuleHalfLen, float capsuleRadius,
	const VECTOR& boxCenter, const VECTOR& boxSize) {
	// カプセル中心線：Y軸方向（回転なし）
	VECTOR p1 = { capsuleCenter.x, capsuleCenter.y + capsuleHalfLen, capsuleCenter.z };
	VECTOR p2 = { capsuleCenter.x, capsuleCenter.y - capsuleHalfLen, capsuleCenter.z };

	// AABB ローカル空間（原点=boxCenter）へ変換
	VECTOR lp1 = VSub(p1, boxCenter);
	VECTOR lp2 = VSub(p2, boxCenter);
	VECTOR half = { boxSize.x * 0.5f, boxSize.y * 0.5f, boxSize.z * 0.5f };

	float distSq = SegmentAabbDistSq_Local(lp1, lp2, half);
	return distSq <= capsuleRadius * capsuleRadius;
}

bool CollisionUtility::SphereAabb(const VECTOR& sphereCenter, float sphereRadius, const VECTOR& boxCenter, const VECTOR& boxSize)
{
	// AABB の半サイズ
	VECTOR half = { boxSize.x * 0.5f, boxSize.y * 0.5f, boxSize.z * 0.5f };

	// 球中心を AABB ローカル（boxCenter 原点）で見て最も近い点を取る
	VECTOR d = VSub(sphereCenter, boxCenter);
	float qx = std::clamp(d.x, -half.x, half.x);
	float qy = std::clamp(d.y, -half.y, half.y);
	float qz = std::clamp(d.z, -half.z, half.z);

	// 最近点をワールドに戻す必要はなく、差分で十分
	VECTOR diff = { d.x - qx, d.y - qy, d.z - qz };
	return VDot(diff, diff) <= sphereRadius * sphereRadius;
}






bool CollisionUtility::SphereAabb_MTV(const VECTOR& C, float R,
	const VECTOR& bmin, const VECTOR& bmax, VECTOR& outN, float& outDepth)
{
	// VoxelBase::SphereVsAABB_MTV 相当  :contentReference[oaicite:11]{index=11}
	VECTOR cp = Utility::Clamp(C, bmin, bmax);
	VECTOR d = VSub(C, cp);
	float L2 = VDot(d, d);
	if (L2 > R * R) return false;
	if (L2 > 1e-12f) {
		float L = std::sqrt(L2);
		outN = VScale(d, 1.0f / L);
		outDepth = R - L;
		return true;
	}
	else {
		float dx = (std::min)(C.x - bmin.x, bmax.x - C.x);
		float dy = (std::min)(C.y - bmin.y, bmax.y - C.y);
		float dz = (std::min)(C.z - bmin.z, bmax.z - C.z);
		if (dx <= dy && dx <= dz) { outN = { (C.x < (bmin.x + bmax.x) * 0.5f) ? -1.f : +1.f, 0, 0 }; outDepth = R + dx; }
		else if (dy <= dz) { outN = { 0, (C.y < (bmin.y + bmax.y) * 0.5f) ? -1.f : +1.f, 0 }; outDepth = R + dy; }
		else { outN = { 0, 0, (C.z < (bmin.z + bmax.z) * 0.5f) ? -1.f : +1.f }; outDepth = R + dz; }
		return true;
	}
}

bool CollisionUtility::CircleXZ_RectXZ_MTV(const VECTOR& C, float R,
	const VECTOR& bmin, const VECTOR& bmax, VECTOR& outN, float& outDepth)
{
	// VoxelBase::CircleXZ_vs_RectXZ_MTV 相当  :contentReference[oaicite:12]{index=12}
	float qx = Utility::Clamp(C.x, bmin.x, bmax.x);
	float qz = Utility::Clamp(C.z, bmin.z, bmax.z);
	float dx = C.x - qx, dz = C.z - qz;
	float L2 = dx * dx + dz * dz;
	if (L2 > R * R) return false;
	if (L2 > 1e-12f) {
		float L = std::sqrt(L2);
		outN = { dx / L, 0.0f, dz / L };
		outDepth = R - L;
		return true;
	}
	else {
		float rx = (std::min)(std::abs(C.x - bmin.x), std::abs(bmax.x - C.x));
		float rz = (std::min)(std::abs(C.z - bmin.z), std::abs(bmax.z - C.z));
		if (rx <= rz) { outN = { (C.x < (bmin.x + bmax.x) * 0.5f) ? -1.f : +1.f, 0, 0 }; outDepth = R + rx; }
		else { outN = { 0, 0, (C.z < (bmin.z + bmax.z) * 0.5f) ? -1.f : +1.f }; outDepth = R + rz; }
		return true;
	}
}

bool CollisionUtility::CapsuleAabbY_MTV(const VECTOR& center, float halfH, float R,
	const VECTOR& bmin, const VECTOR& bmax, VECTOR& outN, float& outDepth)
{
	// 上球・下球
	VECTOR N1; float D1;
	if (SphereAabb_MTV(VAdd(center, VGet(0, +halfH, 0)), R, bmin, bmax, N1, D1)) { outN = N1; outDepth = D1; return true; }
	VECTOR N2; float D2;
	if (SphereAabb_MTV(VAdd(center, VGet(0, -halfH, 0)), R, bmin, bmax, N2, D2)) { outN = N2; outDepth = D2; return true; }

	// 側面（Y が重なっている時のみ XZ 円で判定）
	float yOverlap = (std::min)(bmax.y, center.y + halfH) - (std::max)(bmin.y, center.y - halfH);
	if (yOverlap > 0.0f) {
		VECTOR N3; float D3;
		if (CircleXZ_RectXZ_MTV(center, R, bmin, bmax, N3, D3)) { outN = N3; outDepth = D3; return true; }
	}
	return false;
}







CollisionUtility::ObbInfo CollisionUtility::MakeObb(const VECTOR& pos, const VECTOR& size, const VECTOR& angle)
{
	ObbInfo r = {};

	r.pos = pos;

	r.half = VScale(size, 0.5f);

	r.axis[0] = VTransform({ 1,0,0 }, Utility::MatrixAllMultXYZ({ angle }));
	r.axis[1] = VTransform({ 0,1,0 }, Utility::MatrixAllMultXYZ({ angle }));
	r.axis[2] = VTransform({ 0,0,1 }, Utility::MatrixAllMultXYZ({ angle }));

	return r;
}

float CollisionUtility::PointSegmentDistSq(const VECTOR& p, const VECTOR& a, const VECTOR& b)
{
	VECTOR ab = VSub(b, a);
	VECTOR ap = VSub(p, a);
	float ab2 = VDot(ab, ab);
	if (ab2 <= 1e-8f) {
		VECTOR d = VSub(p, a);
		return VDot(d, d);
	}
	float t = VDot(ap, ab) / ab2;
	if (t < 0.0f) t = 0.0f;
	else if (t > 1.0f) t = 1.0f;
	VECTOR q = VAdd(a, VScale(ab, t));
	VECTOR d = VSub(p, q);
	return VDot(d, d);
}

float CollisionUtility::SegmentSegmentDistSq(const VECTOR& p1, const VECTOR& q1, const VECTOR& p2, const VECTOR& q2)
{
	VECTOR d1 = VSub(q1, p1); // 1の方向
	VECTOR d2 = VSub(q2, p2); // 2の方向
	VECTOR r = VSub(p1, p2);
	float a = VDot(d1, d1);
	float e = VDot(d2, d2);
	float f = VDot(d2, r);

	float s, t;

	if (a <= 1e-8f && e <= 1e-8f) {
		// 双方点
		return VDot(r, r);
	}
	if (a <= 1e-8f) {
		// 1が点
		s = 0.0f;
		t = std::clamp(f / e, 0.0f, 1.0f);
	}
	else {
		float c = VDot(d1, r);
		if (e <= 1e-8f) {
			// 2が点
			t = 0.0f;
			s = std::clamp(-c / a, 0.0f, 1.0f);
		}
		else {
			float b = VDot(d1, d2);
			float denom = a * e - b * b;
			if (denom != 0.0f) s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			else               s = 0.0f;
			float tnom = (b * s + f);
			t = std::clamp(tnom / e, 0.0f, 1.0f);

			// s を再調整
			float sNom = (b * t - c);
			s = std::clamp((denom != 0.0f) ? sNom / a : 0.0f, 0.0f, 1.0f);
		}
	}

	VECTOR c1 = VAdd(p1, VScale(d1, s));
	VECTOR c2 = VAdd(p2, VScale(d2, t));
	VECTOR d = VSub(c1, c2);
	return VDot(d, d);
}

float CollisionUtility::SegmentAabbDistSq_Local(const VECTOR& p0, const VECTOR& p1, const VECTOR& half, int steps)
{
	float best = FLT_MAX;
	for (int i = 0; i <= steps; ++i) {
		float t = (float)i / steps;
		VECTOR p = VAdd(p0, VScale(VSub(p1, p0), t));
		float qx = (p.x < -half.x) ? -half.x : (p.x > half.x ? half.x : p.x);
		float qy = (p.y < -half.y) ? -half.y : (p.y > half.y ? half.y : p.y);
		float qz = (p.z < -half.z) ? -half.z : (p.z > half.z ? half.z : p.z);
		VECTOR q = { qx, qy, qz };
		VECTOR d = VSub(p, q);
		best = (std::min)(best, VDot(d, d));
	}
	return best;
}
