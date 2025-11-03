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

	static void AddDynamicPlayer(UnitBase* obj) { dynamicPlayerObjects_.push_back(obj); }
	static void AddDynamicPlayer(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddDynamicPlayer(o); } }

	static void AddPlayer(UnitBase* obj) { playerObjects_.push_back(obj); }
	static void AddPlayer(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddPlayer(o); } }

	static void AddDynamicEnemy(UnitBase* obj) { dynamicEnemyObjects_.push_back(obj); }
	static void AddDynamicEnemy(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddDynamicEnemy(o); } }

	static void AddEnemy(UnitBase* obj) { enemyObjects_.push_back(obj); }
	static void AddEnemy(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddEnemy(o); } }

	static void AddStage(UnitBase* obj) { stageObject_.push_back(obj); }
	static void AddStage(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddStage(o); } }

	void ResolveDynamics(void);
	void ResolvePairs();
	void SetSlopeLimitDeg(float deg) { slopeLimitDeg_ = deg; }

	void Check();

	void Clear() {
		dynamicPlayerObjects_.clear();
		dynamicEnemyObjects_.clear();
		playerObjects_.clear();
		enemyObjects_.clear();
		stageObject_.clear();
	}

	void SetResolveIters(int n) { resolveIters_ = (n < 1 ? 1 : n); }

private:
	// ‰Ÿ‚µ–ß‚µ‚ª•K—v‚È‚à‚Ì`````````````
	static std::vector<UnitBase*> dynamicPlayerObjects_;
	static std::vector<UnitBase*> dynamicEnemyObjects_;
	//```````````````````````

	// Õ“Ë’Ê’m‚¾‚¯‚ÅÏ‚Ş‚à‚Ì````````````
	static std::vector<UnitBase*> playerObjects_;
	static std::vector<UnitBase*> enemyObjects_;
	static std::vector<UnitBase*> stageObject_;
	//```````````````````````

	float slopeLimitDeg_ = 50.0f; // ‚¾‚¢‚½‚¢‚Ì°Šp

	int resolveIters_ = 5;   // ƒfƒtƒH3‰ñ



	// ---- “à•”‚Åg‚¤gÚGŒó•âh•\Œ» ----
	struct Contact {
		bool   valid = false;
		VECTOR push = { 0,0,0 }; // ‚»‚Ì”½•œ‚Å pos/center ‚É‰Á‚¦‚é‰Ÿ‚µ–ß‚µ
		VECTOR n = { 0,0,0 }; // –@üiƒXƒ‰ƒCƒh—pj
		float  weight = 0.0f;    // ”äŠr—p‚Ìd‚İiVoxel=‰Ÿ‚µ–ß‚µ’·‚³^2, AABB=[‚³j
		bool   grounded = false;
		VECTOR velOut = { 0,0,0 }; // ƒXƒ‰ƒCƒhŒã‚Ì‘¬“xiVoxel‚Å—LŒøj
	};

	bool ResolvePair(class UnitBase* mover, class UnitBase* stage);

	// š3: ƒyƒA‚©‚ç Contact ‚ğ1‚ÂZoigŒó•âh‚¾‚¯B“K—p‚Í‚µ‚È‚¢j
	bool ComputeContactForPair(class UnitBase* mover, class UnitBase* stage,
		const VECTOR& pos, const VECTOR& vel,
		Contact& out) const;

	// ‰º¿‚¯iŒ`ó•ÊjBmover ‚ÍƒJƒvƒZƒ‹‘z’èBstage ‚Í Voxel or AABB ‚ğ‘z’èB
	bool CapsuleVsVoxel_Contact(const Base& du, const VECTOR& pos, const VECTOR& vel,
		class VoxelBase* vox, Contact& out) const;
	bool CapsuleVsAabb_Contact(const Base& du, const VECTOR& pos, const VECTOR& vel,
		const Base& su, Contact& out) const;

	// Õ“ËŠÖ”U‚è•ª‚¯
	bool IsHit(const Base& a, const Base& b);

	// Õ“ËŠÖ”
	bool SphereSphere(const Base& a, const Base& b) const;
	bool ObbObb(const Base& a, const Base& b) const;
	bool AabbAabb(const Base& a, const Base& b) const;
	bool CapsuleCapsule(const Base& a, const Base& b) const;
	bool SphereObb(const Base& sphere, const Base& obb) const;
	bool SphereCapsule(const Base& sphere, const Base& capsule) const;
	bool CapsuleObb(const Base& capsule, const Base& obb) const;
	bool CapsuleAabb(const Base& capsule, const Base& obb) const;
	bool SphereAabb(const Base& sphere, const Base& aabb)const;
};