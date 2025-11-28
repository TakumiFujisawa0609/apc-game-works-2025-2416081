#pragma once

#include<vector>

#include"../../Object/Common/Transform.h"

#include"../../Object/Common/Collider/LineCollider.h"
#include"../../Object/Common/Collider/SphereCollider.h"
#include"../../Object/Common/Collider/CapsuleCollider.h"
#include"../../Object/Common/Collider/BoxCollider.h"
#include"../../Object/Common/Collider/ModelCollider.h"
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

	bool LineToLine(LineCollider* a, LineCollider* b);
	bool SphereToSphere(SphereCollider* a, SphereCollider* b);
	bool CapsuleToCapsule(CapsuleCollider* a, CapsuleCollider* b);
	bool BoxToBox(BoxCollider* a, BoxCollider* b);
	bool ModelToModel(ModelCollider* a, ModelCollider* b);
	bool VoxelToVoxel(VoxelCollider* a, VoxelCollider* b);

	bool LineToSphere(LineCollider* line, SphereCollider* sphere);
	bool LineToCapsule(LineCollider* line, CapsuleCollider* capsule);
	bool LineToBox(LineCollider* line, BoxCollider* box);
	bool LineToModel(LineCollider* line, ModelCollider* model);
	bool LineToVoxel(LineCollider* line, VoxelCollider* voxel);
	bool SphereToCapsule(SphereCollider* sphere, CapsuleCollider* capsule);
	bool SphereToBox(SphereCollider* sphere, BoxCollider* box);
	bool SphereToModel(SphereCollider* sphere, ModelCollider* model);
	bool SphereToVoxel(SphereCollider* sphere, VoxelCollider* voxel);
	bool CapsuleToBox(CapsuleCollider* capsule, BoxCollider* box);
	bool CasuleToModel(CapsuleCollider* capsule, ModelCollider* model);
	bool CapsuleToVoxel(CapsuleCollider* capsule, VoxelCollider* voxel);
	bool BoxToModel(BoxCollider* box, ModelCollider* model);
	bool BoxToVoxel(BoxCollider* box, VoxelCollider* voxel);
	bool ModelToVoxel(ModelCollider* model, VoxelCollider* voxel);

#pragma endregion

#pragma region ユーティリティ
	/// <summary>
	/// 重みづけの割合を計算
	/// </summary>
	/// <param name="aWeight">in 重み</param>
	/// <param name="bWeight">in 重み</param>
	/// <param name="aWeightRatio">out 重みの割合</param>
	/// <param name="bWeightRatio">out 重みの割合</param>
	void WeightRatioCalculation(unsigned char aWeight, unsigned char bWeight, float aWeightRatio, float bWeightRatio) {
		// お互いの重みにおける割合を計算（相手の重み ÷ 自分と相手の重みの合計）
		aWeightRatio = (float)bWeight / (float)(aWeight + bWeight);
		bWeightRatio = (float)aWeight / (float)(aWeight + bWeight);
	}
#pragma endregion

};