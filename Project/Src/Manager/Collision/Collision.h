#pragma once

#include <vector>
#include <DxLib.h>
#include "../../Object/UnitBase.h"

class Collision
{
public:
	Collision();
	~Collision();

	static void AddObject(UnitBase* obj) { objects_.push_back(obj); }
	void AddObject(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddObject(o); } }
	void AddStage(UnitBase* obj) { stageObject_.push_back(obj); }
	void AddStage(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddStage(o); } }

	void Check();
	void Clear() { objects_.clear(); stageObject_.clear(); }

private:
	static std::vector<UnitBase*> objects_;
	static std::vector<UnitBase*> stageObject_;

	// è’ìÀä÷êîêUÇËï™ÇØ
	bool IsHit(const Base& a, const Base& b);

	// è’ìÀä÷êî
	bool SphereSphere(const Base& a, const Base& b) const;
	bool ObbObb(const Base& a, const Base& b) const;
	bool CapsuleCapsule(const Base& a, const Base& b) const;
	bool SphereObb(const Base& sphere, const Base& obb) const;
	bool SphereCapsule(const Base& sphere, const Base& capsule) const;
	bool CapsuleObb(const Base& capsule, const Base& obb) const;
};