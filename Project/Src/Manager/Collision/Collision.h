#pragma once

#include <vector>
#include <DxLib.h>
#include "../../Object/UnitBase.h"

class Collision
{
public:
	Collision();
	~Collision();

	void Add(UnitBase* obj) { objects_.push_back(obj); }
	void Add(std::vector<UnitBase*>obj) { for (auto& o : obj) { Add(o); } }
	void Clear() { objects_.clear(); }
	void Check();

private:
	std::vector<UnitBase*> objects_;

	// Õ“ËŠÖ”U‚è•ª‚¯
	bool IsHit(const Base& a, const Base& b);

	// Õ“ËŠÖ”
	bool SphereSphere(const Base& a, const Base& b) const;
	bool AabbAabb(const Base& a, const Base& b) const;
	bool CapsuleCapsule(const Base& a, const Base& b) const;
	bool SphereAabb(const Base& sphere, const Base& aabb) const;
	bool SphereCapsule(const Base& sphere, const Base& capsule) const;
	bool CapsuleAabb(const Base& capsule, const Base& aabb) const;

	// •â•
	struct Aabb { VECTOR center; VECTOR half; };
	Aabb MakeAabb(const Base& box) const;

};