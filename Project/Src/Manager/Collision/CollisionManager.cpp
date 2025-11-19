#include"CollisionManager.h"

#include<cmath>

#include"CollisionUtility.h"

void CollisionManager::Add(ColliderBase*& collider)
{
	if (!collider) { return; }

	// タイプを見分けて適した配列に格納
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
		if (a->GetJudge() == 0) { continue; }

		for (ColliderBase*& b : bs) {
			if (!b) { continue; }
			if (b->GetJudge() == 1) { continue; }

			if (IsHit(a, b)) {
				a->CallOnCollision(b->GetTag());
				b->CallOnCollision(a->GetTag());
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

	// 形状を判別して適切な関数にて判定を行う～～～～～～～～～～～～～～～～

	// 同形状同士-------------------------------
	
	// 線分同士
	if (aShape == SHAPE::LINE && bShape == SHAPE::LINE) { return false; }

	// 球体同士
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::SPHERE) { return false; }

	// カプセル同士
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::CAPSULE) { return false; }

	// ボックス同士
	if (aShape == SHAPE::AABB && bShape == SHAPE::AABB) { return false; }

	// モデルポリゴン同士
	if (aShape == SHAPE::MODEL && bShape == SHAPE::MODEL) { return false; }

	// ボクセル状同士
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::VOXEL) { return false; }

	// -----------------------------------------
	
	// 別形状同士-------------------------------

	// 線分×球体
	if (aShape == SHAPE::LINE && bShape == SHAPE::SPHERE) { return false; }
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::LINE) { return false; }

	// 線分×カプセル
	if (aShape == SHAPE::LINE && bShape == SHAPE::CAPSULE) { return false; }
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::LINE) { return false; }

	// 線分×ボックス
	if (aShape == SHAPE::LINE && bShape == SHAPE::AABB) { return false; }
	if (aShape == SHAPE::AABB && bShape == SHAPE::LINE) { return false; }

	// 線分×モデルポリゴン
	if (aShape == SHAPE::LINE && bShape == SHAPE::MODEL) { return false; }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::LINE) { return false; }

	// 線分×ボクセル
	if (aShape == SHAPE::LINE && bShape == SHAPE::VOXEL) { return false; }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::LINE) { return false; }

	// 球体×カプセル
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::CAPSULE) { return false; }
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::SPHERE) { return false; }

	// 球体×ボックス
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::AABB) { return false; }
	if (aShape == SHAPE::AABB && bShape == SHAPE::SPHERE) { return false; }

	// 球体×モデルポリゴン
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::MODEL) { return false; }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::SPHERE) { return false; }

	// 球体×ボクセル
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::VOXEL) { return false; }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::SPHERE) { return false; }

	// カプセル×ボックス
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::AABB) { return false; }
	if (aShape == SHAPE::AABB && bShape == SHAPE::CAPSULE) { return false; }

	// カプセル×モデルポリゴン
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::MODEL) { return false; }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::CAPSULE) { return false; }

	// カプセル×ボクセル
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::VOXEL) { return false; }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::CAPSULE) { return false; }

	// ボックス×モデルポリゴン
	if (aShape == SHAPE::AABB && bShape == SHAPE::MODEL) { return false; }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::AABB) { return false; }

	// ボックス×ボクセル
	if (aShape == SHAPE::AABB && bShape == SHAPE::VOXEL) { return false; }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::AABB) { return false; }

	// モデルポリゴン×ボクセル
	if (aShape == SHAPE::MODEL && bShape == SHAPE::VOXEL) { return false; }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::MODEL) { return false; }
	
	// -----------------------------------------
	
	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// どの組み合わせにも属さなかった場合判定なし（falseで返却）
	return false;
}

bool CollisionManager::LineToLine(ColliderLine* a, ColliderLine* b)
{
	// 線分同士の当たり判定
	return false;
}

bool CollisionManager::SphereToSphere(ColliderBase* a, ColliderBase* b)
{
	return false;
}

bool CollisionManager::CapsuleToCapsule(ColliderBase* a, ColliderBase* b)
{
	return false;
}

bool CollisionManager::AabbToAabb(ColliderBase* a, ColliderBase* b)
{
	return false;
}
