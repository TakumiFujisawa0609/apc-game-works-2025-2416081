#pragma once

#include<vector>

#include"../../Object/Common/Transform.h"

#include"../../Object/Common/Collider/LineCollider.h"
#include"../../Object/Common/Collider/SphereCollider.h"
#include"../../Object/Common/Collider/CapsuleCollider.h"
#include"../../Object/Common/Collider/BoxCollider.h"
#include"../../Object/Common/Collider/VoxelCollider.h"

class CollisionManager
{
public:
	CollisionManager() {}
	~CollisionManager() {}

	// オブジェクト追加
	static void Add(ColliderBase*& collider);
	// オブジェクト追加
	static void Add(std::vector<ColliderBase*>& collider) { for (ColliderBase*& c : collider) { Add(c); } }

	// 判定実行
	void Check(void);

private:
#pragma region タイプ別コライダー格納配列
	static std::vector<ColliderBase*>playerColliders_;
	static std::vector<ColliderBase*>enemyColliders_;
	static std::vector<ColliderBase*>stageColliders_;
#pragma endregion

#pragma region 当たり判定用
	void Matching(std::vector<ColliderBase*>& as, std::vector<ColliderBase*>& bs);
	bool IsHit(ColliderBase*& a, ColliderBase*& b);

	bool LineToLine(LineCollider*& a, LineCollider*& b);
	bool SphereToSphere(SphereCollider*& a, SphereCollider*& b);
	bool CapsuleToCapsule(CapsuleCollider*& a, CapsuleCollider*& b);
	bool BoxToBox(BoxCollider*& a, BoxCollider*& b);

#pragma endregion
};