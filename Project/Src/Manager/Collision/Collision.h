#pragma once

#include <vector>
#include <DxLib.h>
#include "../../Object/UnitBase.h"
#include "../../Object/VoxelBase.h"

class Collision
{
public:
	Collision();
	~Collision();

	static void AddObject(UnitBase* obj) { objects_.push_back(obj); }
	void AddObject(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddObject(o); } }
	void AddStage(UnitBase* obj) { stageObject_.push_back(obj); }
	void AddStage(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddStage(o); } }

	void ResolveDynamics(void);      // ★追加：物理ステップ（vel修正＋必要最小押し戻し）
	void ResolvePairs();
	void SetSlopeLimitDeg(float deg) { slopeLimitDeg_ = deg; } // 接地角の許容

	void Check();
	void Clear() { objects_.clear(); stageObject_.clear(); }


	void SetResolveIters(int n) { resolveIters_ = (n < 1 ? 1 : n); }

private:



	static std::vector<UnitBase*> objects_;
	static std::vector<UnitBase*> stageObject_;


	float slopeLimitDeg_ = 50.0f; // だいたいの床角

	int resolveIters_ = 3;   // デフォ3回



	// ---- 内部で使う“接触候補”表現 ----
	struct Contact {
		bool   valid = false;
		VECTOR push = { 0,0,0 }; // その反復で pos/center に加える押し戻し
		VECTOR n = { 0,0,0 }; // 法線（スライド用）
		float  weight = 0.0f;    // 比較用の重み（Voxel=押し戻し長さ^2, AABB=深さ）
		bool   grounded = false;
		VECTOR velOut = { 0,0,0 }; // スライド後の速度（Voxelで有効）
	};

	bool ResolvePair(class UnitBase* mover, class UnitBase* stage);

	// ★3: ペアから Contact を1つ算出（“候補”だけ。適用はしない）
	bool ComputeContactForPair(class UnitBase* mover, class UnitBase* stage,
		const VECTOR& pos, const VECTOR& vel,
		Contact& out) const;

	// 下請け（形状別）。mover はカプセル想定。stage は Voxel or AABB を想定。
	bool CapsuleVsVoxel_Contact(const Base& du, const VECTOR& pos, const VECTOR& vel,
		class VoxelBase* vox, Contact& out) const;
	bool CapsuleVsAabb_Contact(const Base& du, const VECTOR& pos, const VECTOR& vel,
		const Base& su, Contact& out) const;

	// 衝突関数振り分け
	bool IsHit(const Base& a, const Base& b);

	// 衝突関数
	bool SphereSphere(const Base& a, const Base& b) const;
	bool ObbObb(const Base& a, const Base& b) const;
	bool CapsuleCapsule(const Base& a, const Base& b) const;
	bool SphereObb(const Base& sphere, const Base& obb) const;
	bool SphereCapsule(const Base& sphere, const Base& capsule) const;
	bool CapsuleObb(const Base& capsule, const Base& obb) const;
	bool CapsuleAabb(const Base& capsule, const Base& obb) const;
	bool SphereAabb(const Base& sphere, const Base& aabb)const;
};