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
	void SetSlopeLimitDeg(float deg) { slopeLimitDeg_ = deg; } // 接地角の許容

	void Check();
	void Clear() { objects_.clear(); stageObject_.clear(); }


	void SetResolveIters(int n) { resolveIters_ = (n < 1 ? 1 : n); }

private:
	static std::vector<UnitBase*> objects_;
	static std::vector<UnitBase*> stageObject_;


	float slopeLimitDeg_ = 50.0f; // だいたいの床角

	int resolveIters_ = 3;   // デフォ3回

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