#include"CollisionManager.h"

#include<cmath>

#include"CollisionUtility.h"

std::vector<ColliderBase*> CollisionManager::playerColliders_ = {};
std::vector<ColliderBase*> CollisionManager::enemyColliders_ = {};
std::vector<ColliderBase*> CollisionManager::stageColliders_ = {};

void CollisionManager::Add(ColliderBase*& collider)
{
	if (!collider) { return; }

	// タグを見分けて適した配列に格納
	switch (collider->GetTag())
	{
		case TAG::NON:break;

		case TAG::PLAYER:
		case TAG::PLAYER_PUNCH:
		case TAG::PLAYER_GOUGE:
		case TAG::PLAYER_THROWING:
			playerColliders_.emplace_back(collider);
			break;
		case TAG::ENEMY:
		case TAG::BOSS:
		case TAG::GOLEM_ATTACK_FALL:
		case TAG::GOLEM_ATTACK_PSYCHOROCK:
		case TAG::GOLEM_ATTACK_STONE:
			enemyColliders_.emplace_back(collider);
			break;
		case TAG::STAGE:
			stageColliders_.emplace_back(collider);
			break;
	}
}

void CollisionManager::Check(void)
{
	// プレイヤー×ステージ
	Matching(playerColliders_, stageColliders_);
	
	// エネミー×ステージ
	Matching(enemyColliders_, stageColliders_);

	// プレイヤー×エネミー
	Matching(playerColliders_, enemyColliders_);
}

void CollisionManager::Matching(std::vector<ColliderBase*>& as, std::vector<ColliderBase*>& bs)
{
	for (ColliderBase*& a : as) {
		if (!a) { continue; }
		if (!a->GetJudge()) { continue; }

		for (ColliderBase*& b : bs) {
			if (!b) { continue; }
			if (!b->GetJudge() == false) { continue; }

			if (IsHit(a, b)) {
				a->CallOnCollision(*b);
				b->CallOnCollision(*a);
			}
		}
	}
}

bool CollisionManager::IsHit(ColliderBase*& a, ColliderBase*& b)
{
	// ローカル変数で各形状を保持（ゲット関数の呼び出しを1回で済ませるため）
	SHAPE aShape = a->GetShape(), bShape = b->GetShape();

	// どちらか、あるいは両方の形状が未設定だったら判定なし（falseで返却）
	if (aShape == SHAPE::NON || bShape == SHAPE::NON) { return false; }

	// お互いの距離による雑な判定スキップ
	float enoughDisSub = a->GetEnoughDistance() + b->GetEnoughDistance();
	if (enoughDisSub > 0.0f) {
		if ((a->GetPos() - b->GetPos()).LengthSq() > std::pow(enoughDisSub, 2.0f)) { return false; }
	}

#pragma region 形状を判別して適切な関数にて判定を行う

	// 同形状同士-------------------------------
	
	// 線分同士
	if (aShape == SHAPE::LINE && bShape == SHAPE::LINE) { return LineToLine(dynamic_cast<LineCollider*>(a), dynamic_cast<LineCollider*>(b)); }

	// 球体同士
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::SPHERE) { return SphereToSphere(dynamic_cast<SphereCollider*>(a), dynamic_cast<SphereCollider*>(b)); }

	// カプセル同士
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::CAPSULE) { return CapsuleToCapsule(dynamic_cast<CapsuleCollider*>(a), dynamic_cast<CapsuleCollider*>(b)); }

	// ボックス同士
	if (aShape == SHAPE::BOX && bShape == SHAPE::BOX) { return BoxToBox(dynamic_cast<BoxCollider*>(a), dynamic_cast<BoxCollider*>(b)); }

	// モデルポリゴン同士
	if (aShape == SHAPE::MODEL && bShape == SHAPE::MODEL) { return ModelToModel(dynamic_cast<ModelCollider*>(a), dynamic_cast<ModelCollider*>(b)); }

	// ボクセル状同士
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::VOXEL) { return VoxelToVoxel(dynamic_cast<VoxelCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }

	// -----------------------------------------
	
	// 別形状同士-------------------------------

	// 線分×球体
	if (aShape == SHAPE::LINE && bShape == SHAPE::SPHERE) { return LineToSphere(dynamic_cast<LineCollider*>(a), dynamic_cast<SphereCollider*>(b)); }
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::LINE) { return LineToSphere(dynamic_cast<LineCollider*>(b), dynamic_cast<SphereCollider*>(a)); }

	// 線分×カプセル
	if (aShape == SHAPE::LINE && bShape == SHAPE::CAPSULE) { return LineToCapsule(dynamic_cast<LineCollider*>(a), dynamic_cast<CapsuleCollider*>(b)); }
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::LINE) { return LineToCapsule(dynamic_cast<LineCollider*>(b), dynamic_cast<CapsuleCollider*>(a)); }

	// 線分×ボックス
	if (aShape == SHAPE::LINE && bShape == SHAPE::BOX) { return LineToBox(dynamic_cast<LineCollider*>(a), dynamic_cast<BoxCollider*>(b)); }
	if (aShape == SHAPE::BOX && bShape == SHAPE::LINE) { return LineToBox(dynamic_cast<LineCollider*>(b), dynamic_cast<BoxCollider*>(a)); }

	// 線分×モデルポリゴン
	if (aShape == SHAPE::LINE && bShape == SHAPE::MODEL) { return LineToModel(dynamic_cast<LineCollider*>(a), dynamic_cast<ModelCollider*>(b)); }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::LINE) { return LineToModel(dynamic_cast<LineCollider*>(b), dynamic_cast<ModelCollider*>(a)); }

	// 線分×ボクセル
	if (aShape == SHAPE::LINE && bShape == SHAPE::VOXEL) { return LineToVoxel(dynamic_cast<LineCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::LINE) { return LineToVoxel(dynamic_cast<LineCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }

	// 球体×カプセル
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::CAPSULE) { return SphereToCapsule(dynamic_cast<SphereCollider*>(a), dynamic_cast<CapsuleCollider*>(b)); }
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::SPHERE) { return SphereToCapsule(dynamic_cast<SphereCollider*>(b), dynamic_cast<CapsuleCollider*>(a)); }

	// 球体×ボックス
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::BOX) { return SphereToBox(dynamic_cast<SphereCollider*>(a), dynamic_cast<BoxCollider*>(b)); }
	if (aShape == SHAPE::BOX && bShape == SHAPE::SPHERE) { return SphereToBox(dynamic_cast<SphereCollider*>(b), dynamic_cast<BoxCollider*>(a)); }

	// 球体×モデルポリゴン
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::MODEL) { return SphereToModel(dynamic_cast<SphereCollider*>(a), dynamic_cast<ModelCollider*>(b)); }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::SPHERE) { return SphereToModel(dynamic_cast<SphereCollider*>(b), dynamic_cast<ModelCollider*>(a)); }

	// 球体×ボクセル
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::VOXEL) { return SphereToVoxel(dynamic_cast<SphereCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::SPHERE) { return SphereToVoxel(dynamic_cast<SphereCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }

	// カプセル×ボックス
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::BOX) { return CapsuleToBox(dynamic_cast<CapsuleCollider*>(a), dynamic_cast<BoxCollider*>(b)); }
	if (aShape == SHAPE::BOX && bShape == SHAPE::CAPSULE) { return CapsuleToBox(dynamic_cast<CapsuleCollider*>(b), dynamic_cast<BoxCollider*>(a)); }

	// カプセル×モデルポリゴン
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::MODEL) { return SphereToModel(dynamic_cast<SphereCollider*>(a), dynamic_cast<ModelCollider*>(b)); }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::CAPSULE) { return SphereToModel(dynamic_cast<SphereCollider*>(b), dynamic_cast<ModelCollider*>(a)); }

	// カプセル×ボクセル
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::VOXEL) { return SphereToVoxel(dynamic_cast<SphereCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::CAPSULE) { return SphereToVoxel(dynamic_cast<SphereCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }

	// ボックス×モデルポリゴン
	if (aShape == SHAPE::BOX && bShape == SHAPE::MODEL) { return BoxToModel(dynamic_cast<BoxCollider*>(a), dynamic_cast<ModelCollider*>(b)); }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::BOX) { return BoxToModel(dynamic_cast<BoxCollider*>(b), dynamic_cast<ModelCollider*>(a)); }

	// ボックス×ボクセル
	if (aShape == SHAPE::BOX && bShape == SHAPE::VOXEL) { return BoxToVoxel(dynamic_cast<BoxCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::BOX) { return BoxToVoxel(dynamic_cast<BoxCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }

	// モデルポリゴン×ボクセル
	if (aShape == SHAPE::MODEL && bShape == SHAPE::VOXEL) { return ModelToVoxel(dynamic_cast<ModelCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::MODEL) { return ModelToVoxel(dynamic_cast<ModelCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }
	
	// -----------------------------------------

#pragma endregion
	
	// どの組み合わせにも属さなかった場合判定なし（falseで返却）
	return false;
}

bool CollisionManager::LineToLine(LineCollider* a, LineCollider* b)
{
	return false;
}

bool CollisionManager::SphereToSphere(SphereCollider* a, SphereCollider* b)
{
	return false;
}

bool CollisionManager::CapsuleToCapsule(CapsuleCollider* a, CapsuleCollider* b)
{
	return false;
}

bool CollisionManager::BoxToBox(BoxCollider* a, BoxCollider* b)
{
	return false;
}

bool CollisionManager::ModelToModel(ModelCollider* a, ModelCollider* b)
{
	return false;
}

bool CollisionManager::VoxelToVoxel(VoxelCollider* a, VoxelCollider* b)
{
	return false;
}

bool CollisionManager::LineToSphere(LineCollider* line, SphereCollider* sphere)
{
	return false;
}

bool CollisionManager::LineToCapsule(LineCollider* line, CapsuleCollider* capsule)
{
	return false;
}

bool CollisionManager::LineToBox(LineCollider* line, BoxCollider* box)
{
	return false;
}

bool CollisionManager::LineToModel(LineCollider* line, ModelCollider* model)
{
	return false;
}

bool CollisionManager::LineToVoxel(LineCollider* line, VoxelCollider* voxel)
{
	return false;
}

bool CollisionManager::SphereToCapsule(SphereCollider* sphere, CapsuleCollider* capsule)
{
	return false;
}

bool CollisionManager::SphereToBox(SphereCollider* sphere, BoxCollider* box)
{
	return false;
}

bool CollisionManager::SphereToModel(SphereCollider* sphere, ModelCollider* model)
{
	return false;
}

bool CollisionManager::SphereToVoxel(SphereCollider* sphere, VoxelCollider* voxel)
{
	return false;
}

bool CollisionManager::CapsuleToBox(CapsuleCollider* capsule, BoxCollider* box)
{
	return false;
}

bool CollisionManager::CasuleToModel(CapsuleCollider* capsule, ModelCollider* model)
{
	return false;
}

bool CollisionManager::CapsuleToVoxel(CapsuleCollider* capsule, VoxelCollider* voxel)
{
	return false;
}

bool CollisionManager::BoxToModel(BoxCollider* box, ModelCollider* model)
{
	return false;
}

bool CollisionManager::BoxToVoxel(BoxCollider* box, VoxelCollider* voxel)
{
	return false;
}

bool CollisionManager::ModelToVoxel(ModelCollider* model, VoxelCollider* voxel)
{
	return false;
}
