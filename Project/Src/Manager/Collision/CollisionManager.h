#pragma once

#include<vector>

#include"../../Object/Common/Transform.h"

#include"../../Object/Common/Collider/ColliderBase.h"
#include"../../Object/Common/Collider/ColliderLine.h"

class CollisionManager
{
public:
	CollisionManager() :
		playerColliders_(),
		enemyColliders_(),
		stageColliders_()
	{
	}
	~CollisionManager() {}

	// オブジェクト追加
	void Add(ColliderBase*& collider);
	// オブジェクト追加
	void Add(std::vector<ColliderBase*>& collider) { for (ColliderBase*& c : collider) { Add(c); } }

	// 判定実行
	void Check(void);

private:
#pragma region タイプ別コライダー格納配列
	std::vector<ColliderBase*>playerColliders_;
	std::vector<ColliderBase*>enemyColliders_;
	std::vector<ColliderBase*>stageColliders_;
#pragma endregion

#pragma region 当たり判定用
	void Matching(std::vector<ColliderBase*>& as, std::vector<ColliderBase*>& bs);
	bool IsHit(ColliderBase*& a, ColliderBase*& b);

	bool LineToLine(ColliderLine* a, ColliderLine* b);
	bool SphereToSphere(ColliderBase* a, ColliderBase* b);
	bool CapsuleToCapsule(ColliderBase* a, ColliderBase* b);
	bool AabbToAabb(ColliderBase* a, ColliderBase* b);

#pragma endregion
};