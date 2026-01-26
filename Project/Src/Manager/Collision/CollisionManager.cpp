#include"CollisionManager.h"

#include<cmath>

std::vector<ColliderBase*> CollisionManager::playerColliders_ = {};
std::vector<ColliderBase*> CollisionManager::enemyColliders_ = {};
std::vector<ColliderBase*> CollisionManager::stageColliders_ = {};
std::vector<ColliderBase*> CollisionManager::otherColliders_ = {};

void CollisionManager::Add(ColliderBase* collider)
{
	if (!collider) { return; }

	// ƒ^ƒO‚ğŒ©•ª‚¯‚Ä“K‚µ‚½”z—ñ‚ÉŠi”[
	switch (collider->GetTag())
	{
		// –¢İ’èi—áŠOˆ—j
	case TAG::NON:  break;

		//ƒvƒŒƒCƒ„[Œn
	case TAG::PLAYER:
	case TAG::PLAYER_PUNCH:
	case TAG::PLAYER_GOUGE:
	case TAG::PLAYER_THROWING:
		playerColliders_.emplace_back(collider);
		break;

		// ƒGƒlƒ~[Œn
	case TAG::BOSS:

	case TAG::ENEMY:

	case TAG::GOLEM_ATTACK_FALL:
	case TAG::GOLEM_ATTACK_PSYCHOROCK:
	case TAG::GOLEM_ATTACK_STONE:
		enemyColliders_.emplace_back(collider);
		break;

		// ƒXƒe[ƒWŒn
	case TAG::STAGE:
	case TAG::GOLEM_ATTACK_WALL:
		stageColliders_.emplace_back(collider);
		break;

		// ‚»‚êˆÈŠO
	default:
		otherColliders_.emplace_back(collider);
		break;
	}
}

void CollisionManager::Check(void)
{
	// ƒvƒŒƒCƒ„[Œn~ƒXƒe[ƒWŒn
	Matching(playerColliders_, stageColliders_);

	// ƒGƒlƒ~[Œn~ƒXƒe[ƒWŒn
	Matching(enemyColliders_, stageColliders_);

	// ‚»‚êˆÈŠO~ƒXƒe[ƒWŒn
	Matching(otherColliders_, stageColliders_);
	
	// ƒvƒŒƒCƒ„[Œn~ƒGƒlƒ~[Œn
	Matching(playerColliders_, enemyColliders_);
	
	// ƒvƒŒƒCƒ„[Œn~‚»‚êˆÈŠO
	Matching(playerColliders_, otherColliders_);

	// ƒGƒlƒ~[Œn~‚»‚êˆÈŠO
	Matching(enemyColliders_, otherColliders_);

	// ‚»‚êˆÈŠO~‚»‚êˆÈŠO
	Matching(otherColliders_);
}

bool CollisionManager::IsStageCollision(const Vector3& pos, float radius, TAG tag)
{
	float r2 = radius * radius;

	for (ColliderBase*& collider : stageColliders_) {
		if (tag == collider->GetTag()) { continue; }

		if (VoxelCollider* voxel = dynamic_cast<VoxelCollider*>(collider)) {
			// --- Rough AABB ---
			Vector3 half = voxel->GetRoughSize() * 0.5f;
			Vector3 cpos = voxel->GetPos();

			Vector3 nearest(
				std::clamp(pos.x, cpos.x - half.x, cpos.x + half.x),
				std::clamp(pos.y, cpos.y - half.y, cpos.y + half.y),
				std::clamp(pos.z, cpos.z - half.z, cpos.z + half.z)
			);

			if ((pos - nearest).LengthSq() > r2) { continue; }

			// --- Cell check ---
			float cellHalf = voxel->GetCellSize() * 0.5f;

			for (std::pair<const int, Vector3>& cell : voxel->GetCellWorldPoss()) {
				Vector3 nearestCell(
					std::clamp(pos.x, cell.second.x - cellHalf, cell.second.x + cellHalf),
					std::clamp(pos.y, cell.second.y - cellHalf, cell.second.y + cellHalf),
					std::clamp(pos.z, cell.second.z - cellHalf, cell.second.z + cellHalf)
				);

				if ((pos - nearestCell).LengthSq() <= r2) { return true; }
			}
		}
	}
	return false;
}

void CollisionManager::Matching(std::vector<ColliderBase*>& as, std::vector<ColliderBase*>& bs)
{
	for (ColliderBase*& a : as) {
		if (!a) { continue; }
		if (!a->GetJudge()) { continue; }

		for (ColliderBase*& b : bs) {
			if (!b) { continue; }
			if (!b->GetJudge()) { continue; }

			if (IsHit(a, b)) {
				a->CallOnCollision(*b);
				b->CallOnCollision(*a);
			}
		}
	}
}

void CollisionManager::Matching(std::vector<ColliderBase*>& s)
{
	unsigned short size = (unsigned short)s.size();

	for (unsigned short a = 0; a < (size - 1); a++) {
		if (!s[a]) { continue; }
		if (!s[a]->GetJudge()) { continue; }

		for (unsigned short b = a + 1; b < size; b++) {
			if (!s[b]) { continue; }
			if (!s[b]->GetJudge()) { continue; }

			if (IsHit(s[a], s[b])) {
				s[a]->CallOnCollision(*s[b]);
				s[b]->CallOnCollision(*s[a]);
			}
		}
	}
}

bool CollisionManager::IsHit(ColliderBase* a, ColliderBase* b)
{
	// ƒ[ƒJƒ‹•Ï”‚ÅŠeŒ`ó‚ğ•ÛiƒQƒbƒgŠÖ”‚ÌŒÄ‚Ño‚µ‚ğ1‰ñ‚ÅÏ‚Ü‚¹‚é‚½‚ßj
	const SHAPE aShape = a->GetShape(), bShape = b->GetShape();

	// ‚Ç‚¿‚ç‚©A‚ ‚é‚¢‚Í—¼•û‚ÌŒ`ó‚ª–¢İ’è‚¾‚Á‚½‚ç”»’è‚È‚µifalse‚Å•Ô‹pj
	if (aShape == SHAPE::NON || bShape == SHAPE::NON) { return false; }

	// ‚»‚à‚»‚àA“¯‚¶ƒ^ƒO“¯m‚Í“–‚½‚è”»’è‚µ‚È‚¢
	if (a->GetTag() == b->GetTag()) { return false; }

	// ‚¨Œİ‚¢‚Ì‹——£‚É‚æ‚éG‚È”»’èƒXƒLƒbƒviŒy—Ê‰»–Ú“Ij
	float enoughDisSub = a->GetEnoughDistance() + b->GetEnoughDistance();
	if (enoughDisSub > 0.0f) {
		if ((a->GetPos() - b->GetPos()).LengthSq() > enoughDisSub * enoughDisSub) { return false; }
	}

#pragma region Œ`ó‚ğ”»•Ê‚µ‚Ä“KØ‚ÈŠÖ”‚É‚Ä”»’è‚ğs‚¤

	// “¯Œ`ó“¯m-------------------------------
	
	// ü•ª“¯m
	if (aShape == SHAPE::LINE && bShape == SHAPE::LINE) { return LineToLine(dynamic_cast<LineCollider*>(a), dynamic_cast<LineCollider*>(b)); }

	// ‹…‘Ì“¯m
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::SPHERE) { return SphereToSphere(dynamic_cast<SphereCollider*>(a), dynamic_cast<SphereCollider*>(b)); }

	// ƒJƒvƒZƒ‹“¯m
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::CAPSULE) { return CapsuleToCapsule(dynamic_cast<CapsuleCollider*>(a), dynamic_cast<CapsuleCollider*>(b)); }

	// ƒ{ƒbƒNƒX“¯m
	if (aShape == SHAPE::BOX && bShape == SHAPE::BOX) { return BoxToBox(dynamic_cast<BoxCollider*>(a), dynamic_cast<BoxCollider*>(b)); }

	// ƒ‚ƒfƒ‹ƒ|ƒŠƒSƒ““¯m
	if (aShape == SHAPE::MODEL && bShape == SHAPE::MODEL) { return ModelToModel(dynamic_cast<ModelCollider*>(a), dynamic_cast<ModelCollider*>(b)); }

	// ƒ{ƒNƒZƒ‹ó“¯m
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::VOXEL) { return VoxelToVoxel(dynamic_cast<VoxelCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }

	// -----------------------------------------
	
	// •ÊŒ`ó“¯m-------------------------------

	// ü•ª~‹…‘Ì
	if (aShape == SHAPE::LINE && bShape == SHAPE::SPHERE) { return LineToSphere(dynamic_cast<LineCollider*>(a), dynamic_cast<SphereCollider*>(b)); }
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::LINE) { return LineToSphere(dynamic_cast<LineCollider*>(b), dynamic_cast<SphereCollider*>(a)); }

	// ü•ª~ƒJƒvƒZƒ‹
	if (aShape == SHAPE::LINE && bShape == SHAPE::CAPSULE) { return LineToCapsule(dynamic_cast<LineCollider*>(a), dynamic_cast<CapsuleCollider*>(b)); }
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::LINE) { return LineToCapsule(dynamic_cast<LineCollider*>(b), dynamic_cast<CapsuleCollider*>(a)); }

	// ü•ª~ƒ{ƒbƒNƒX
	if (aShape == SHAPE::LINE && bShape == SHAPE::BOX) { return LineToBox(dynamic_cast<LineCollider*>(a), dynamic_cast<BoxCollider*>(b)); }
	if (aShape == SHAPE::BOX && bShape == SHAPE::LINE) { return LineToBox(dynamic_cast<LineCollider*>(b), dynamic_cast<BoxCollider*>(a)); }

	// ü•ª~ƒ‚ƒfƒ‹ƒ|ƒŠƒSƒ“
	if (aShape == SHAPE::LINE && bShape == SHAPE::MODEL) { return LineToModel(dynamic_cast<LineCollider*>(a), dynamic_cast<ModelCollider*>(b)); }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::LINE) { return LineToModel(dynamic_cast<LineCollider*>(b), dynamic_cast<ModelCollider*>(a)); }

	// ü•ª~ƒ{ƒNƒZƒ‹
	if (aShape == SHAPE::LINE && bShape == SHAPE::VOXEL) { return LineToVoxel(dynamic_cast<LineCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::LINE) { return LineToVoxel(dynamic_cast<LineCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }

	// ‹…‘Ì~ƒJƒvƒZƒ‹
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::CAPSULE) { return SphereToCapsule(dynamic_cast<SphereCollider*>(a), dynamic_cast<CapsuleCollider*>(b)); }
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::SPHERE) { return SphereToCapsule(dynamic_cast<SphereCollider*>(b), dynamic_cast<CapsuleCollider*>(a)); }

	// ‹…‘Ì~ƒ{ƒbƒNƒX
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::BOX) { return SphereToBox(dynamic_cast<SphereCollider*>(a), dynamic_cast<BoxCollider*>(b)); }
	if (aShape == SHAPE::BOX && bShape == SHAPE::SPHERE) { return SphereToBox(dynamic_cast<SphereCollider*>(b), dynamic_cast<BoxCollider*>(a)); }

	// ‹…‘Ì~ƒ‚ƒfƒ‹ƒ|ƒŠƒSƒ“
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::MODEL) { return SphereToModel(dynamic_cast<SphereCollider*>(a), dynamic_cast<ModelCollider*>(b)); }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::SPHERE) { return SphereToModel(dynamic_cast<SphereCollider*>(b), dynamic_cast<ModelCollider*>(a)); }

	// ‹…‘Ì~ƒ{ƒNƒZƒ‹
	if (aShape == SHAPE::SPHERE && bShape == SHAPE::VOXEL) { return SphereToVoxel(dynamic_cast<SphereCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::SPHERE) { return SphereToVoxel(dynamic_cast<SphereCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }

	// ƒJƒvƒZƒ‹~ƒ{ƒbƒNƒX
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::BOX) { return CapsuleToBox(dynamic_cast<CapsuleCollider*>(a), dynamic_cast<BoxCollider*>(b)); }
	if (aShape == SHAPE::BOX && bShape == SHAPE::CAPSULE) { return CapsuleToBox(dynamic_cast<CapsuleCollider*>(b), dynamic_cast<BoxCollider*>(a)); }

	// ƒJƒvƒZƒ‹~ƒ‚ƒfƒ‹ƒ|ƒŠƒSƒ“
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::MODEL) { return SphereToModel(dynamic_cast<SphereCollider*>(a), dynamic_cast<ModelCollider*>(b)); }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::CAPSULE) { return SphereToModel(dynamic_cast<SphereCollider*>(b), dynamic_cast<ModelCollider*>(a)); }

	// ƒJƒvƒZƒ‹~ƒ{ƒNƒZƒ‹
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::VOXEL) { return CapsuleToVoxel(dynamic_cast<CapsuleCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::CAPSULE) { return CapsuleToVoxel(dynamic_cast<CapsuleCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }

	// ƒ{ƒbƒNƒX~ƒ‚ƒfƒ‹ƒ|ƒŠƒSƒ“
	if (aShape == SHAPE::BOX && bShape == SHAPE::MODEL) { return BoxToModel(dynamic_cast<BoxCollider*>(a), dynamic_cast<ModelCollider*>(b)); }
	if (aShape == SHAPE::MODEL && bShape == SHAPE::BOX) { return BoxToModel(dynamic_cast<BoxCollider*>(b), dynamic_cast<ModelCollider*>(a)); }

	// ƒ{ƒbƒNƒX~ƒ{ƒNƒZƒ‹
	if (aShape == SHAPE::BOX && bShape == SHAPE::VOXEL) { return BoxToVoxel(dynamic_cast<BoxCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::BOX) { return BoxToVoxel(dynamic_cast<BoxCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }

	// ƒ‚ƒfƒ‹ƒ|ƒŠƒSƒ“~ƒ{ƒNƒZƒ‹
	if (aShape == SHAPE::MODEL && bShape == SHAPE::VOXEL) { return ModelToVoxel(dynamic_cast<ModelCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::MODEL) { return ModelToVoxel(dynamic_cast<ModelCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }
	
	// -----------------------------------------

#pragma endregion
	
	// ‚Ç‚Ì‘g‚İ‡‚í‚¹‚É‚à‘®‚³‚È‚©‚Á‚½ê‡”»’è‚È‚µifalse‚Å•Ô‹pj
	return false;
}

bool CollisionManager::LineToLine(LineCollider* a, LineCollider* b)
{
	return false;
}

bool CollisionManager::SphereToSphere(SphereCollider* a, SphereCollider* b)
{
#pragma region •K—vî•ñ‚ğ‹‚ß‚é
	// ƒxƒNƒgƒ‹
	Vector3 normal = a->GetPos() - b->GetPos();
	// ”¼Œa‚Ì‡Œv
	float radius = a->GetRadius() + b->GetRadius();
#pragma endregion

#pragma region Õ“Ë”»’èi‚Q“_ŠÔ‚Ì‹——£‚ğ‚Í‚©‚Á‚Ä –¢Õ“Ë‚È‚çI—¹j
	// ƒxƒNƒgƒ‹‚Ì’·‚³‚Ì‚Qæ‚Æ”¼Œa‚Ì‡Œv‚Ì‚Qæ‚ğ”ä‚×‚Ä”»’èi–¢Õ“Ë‚È‚çI—¹j
	if (normal.LengthSq() > radius * radius) { return false; }

#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	// ‚Q‚Â‚Æ‚à‰Ÿ‚µo‚µ‚ğs‚¤ƒIƒuƒWƒFƒNƒg‚Ìê‡A‚ß‚è‚ñ‚¾—Ê‚ğŒ©‚Ä‰Ÿ‚µo‚·
	if (NeedPush(a,b)) {
		// ‚ß‚è‚ñ‚¾—Ê
		float overrap = radius - normal.Length();

		if (normal == 0.0f) {
			Vector3 velocity = a->GetTransform().Velocity();
			normal = (velocity != 0.0f) ? velocity : Vector3(0.0f, 1.0f, 0.0f);
		}

		// ‰Ÿ‚µo‚µˆ—
		ApplyPush(a, b, normal.Normalized(), overrap);
	}
#pragma endregion

	// “–‚½‚Á‚½
	return true;
}

bool CollisionManager::CapsuleToCapsule(CapsuleCollider* a, CapsuleCollider* b)
{
#pragma region •K—vî•ñ‚ğæ“¾
	// ü•ª‚Ì n“_/I“_ 
	const Vector3 aStartPos = a->GetStartPos(), aEndPos = a->GetEndPos();
	const Vector3 bStartPos = b->GetStartPos(), bEndPos = b->GetEndPos();

	// ”¼Œa
	const float   aRadius = a->GetRadius(), bRadius = b->GetRadius();
#pragma endregion

#pragma region Õ“Ë”»’èi‚¨Œİ‚¢‚Ìü•ªã‚É‚¨‚¯‚éÅ‹ß“_‚ğ‹‚ß‚Ä‚»‚Ì‚Q“_ŠÔ‚Ì‹——£‚ğ‚Í‚©‚Á‚Ä –¢Õ“Ë‚È‚çI—¹j
	// ü•ª“¯m‚ÌÅ‹ßÚ“_‚ğ‹‚ß‚é ```````````

	// ‚±‚±‚ÉÅ‹ßÚ“_‚ª“ü‚é
	Vector3 pa = {}, pb = {};

	// A‚Ì•ûŒüƒxƒNƒgƒ‹
	Vector3 u = aEndPos - aStartPos;
	// B‚Ì•ûŒüƒxƒNƒgƒ‹
	Vector3 v = bEndPos - bStartPos;

	// B‚Ìn“_‚©‚çA‚Ìn“_‚Ü‚Å‚ÌƒxƒNƒgƒ‹
	Vector3 w = aStartPos - bStartPos;

	float aLen = u.LengthSq();
	float bLen = v.LengthSq();
	float ab = u.Dot(v);
	float aw = u.Dot(w);
	float bw = v.Dot(w);

	float denom = aLen * bLen - ab * ab;
	float s, t;

	if (denom < 1e-6f)	{
		// ü•ª‚ª‚Ù‚Ú•½s ¨ •Ğ•û‚É‡‚í‚¹‚ÄŒvZ
		s = 0.0f;
		t = bw / bLen;
	}
	else {
		s = (ab * bw - bLen * aw) / denom;
		t = (aLen * bw - ab * aw) / denom;
	}

	// ü•ª“à‚É clamp
	s = std::clamp(s, 0.0f, 1.0f);
	t = std::clamp(t, 0.0f, 1.0f);

	pa = aStartPos + u * s;  // Aü•ªã‚ÌÅ‹ß“_
	pb = bStartPos + v * t;  // Bü•ªã‚ÌÅ‹ß“_

	// ‹——£ŒvZ
	Vector3 normal = pa - pb;
	float distSq = normal.LengthSq();
	float radSum = aRadius + bRadius;

	// 
	if (distSq >= radSum * radSum) { return false; }

#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	// ‰Ÿ‚µo‚µ‚ª•K—v‚©‚Ç‚¤‚©
	if (NeedPush(a, b)) {

		float dist = std::sqrt(distSq);
		if (dist < 1e-6f) {
			// ƒ[ƒ‹——£
			normal = -a->GetTransform().Velocity();
			dist = 0.0f;
		}

		// ‚ß‚è‚İ—Ê
		float overlap = radSum - dist;

		// ‰Ÿ‚µo‚µˆ—
		ApplyPush(a, b, normal.Normalized(), overlap);
	}
#pragma endregion

	// “–‚½‚Á‚½
	return true;
}

bool CollisionManager::BoxToBox(BoxCollider* a, BoxCollider* b)
{
#pragma region •K—vî•ñ‚ğæ“¾
	Vector3 normal = a->GetPos() - b->GetPos();
	Vector3 halfSizeSum = (a->GetSize() + b->GetSize()) * 0.5f;
#pragma endregion

#pragma region Õ“Ë”»’èij
	if (abs(normal.x) > halfSizeSum.x) { return false; }
	if (abs(normal.y) > halfSizeSum.y) { return false; }
	if (abs(normal.z) > halfSizeSum.z) { return false; }
#pragma endregion

#pragma region Õ“ËŠm’èF•K—v‚È‚ç‰Ÿ‚µo‚µ
	if (NeedPush(a, b)) {

		Vector3 overlapNorm = halfSizeSum - normal.Abs();

		ApplyPush(a, b, overlapNorm.MinElement() * (normal / normal.Abs()));
	}
#pragma endregion

	return true;
}

bool CollisionManager::ModelToModel(ModelCollider* a, ModelCollider* b)
{
	return false;
}

#pragma region Š®‘S”Åiˆ—‚•‰‰×j
//bool CollisionManager::VoxelToVoxel(VoxelCollider* a, VoxelCollider* b)
//{
//	a->ClearHitCellIdxs();
//	b->ClearHitCellIdxs();
//
//#pragma region ‡@ rough AABB ”»’è
//	Vector3 aPos = a->GetPos();
//	Vector3 bPos = b->GetPos();
//
//	Vector3 aHalf = a->GetRoughSize() * 0.5f;
//	Vector3 bHalf = b->GetRoughSize() * 0.5f;
//
//	if (fabs(aPos.x - bPos.x) > aHalf.x + bHalf.x) return false;
//	if (fabs(aPos.y - bPos.y) > aHalf.y + bHalf.y) return false;
//	if (fabs(aPos.z - bPos.z) > aHalf.z + bHalf.z) return false;
//#pragma endregion
//
//#pragma region ‡A ƒZƒ‹’PˆÊ”»’è
//	float cellHalfA = a->GetCellSize() * 0.5f;
//	float cellHalfB = b->GetCellSize() * 0.5f;
//
//	bool hit = false;
//
//	float maxOverlap = -FLT_MAX;
//	Vector3 bestNormal{};
//
//	int bestIdxA = -1;
//	int bestIdxB = -1;
//
//	for (auto& ca : a->GetCellWorldPoss())
//	{
//		Vector3 aMin = ca.second - Vector3(cellHalfA);
//		Vector3 aMax = ca.second + Vector3(cellHalfA);
//
//		for (auto& cb : b->GetCellWorldPoss())
//		{
//			Vector3 bMin = cb.second - Vector3(cellHalfB);
//			Vector3 bMax = cb.second + Vector3(cellHalfB);
//
//			// AABB vs AABB
//			float ox = (std::min)(aMax.x, bMax.x) - (std::max)(aMin.x, bMin.x);
//			float oy = (std::min)(aMax.y, bMax.y) - (std::max)(aMin.y, bMin.y);
//			float oz = (std::min)(aMax.z, bMax.z) - (std::max)(aMin.z, bMin.z);
//
//			if (ox <= 0 || oy <= 0 || oz <= 0) continue;
//
//			hit = true;
//
//			a->AddHitCellIdx(ca.first);
//			b->AddHitCellIdx(cb.first);
//
//			// ‰Ÿ‚µo‚µ•ûŒüŒó•â
//			float minOverlap = ox;
//			Vector3 n = Vector3((ca.second.x > cb.second.x) ? 1 : -1, 0, 0);
//
//			if (oy < minOverlap)
//			{
//				minOverlap = oy;
//				n = Vector3(0, (ca.second.y > cb.second.y) ? 1 : -1, 0);
//			}
//			if (oz < minOverlap)
//			{
//				minOverlap = oz;
//				n = Vector3(0, 0, (ca.second.z > cb.second.z) ? 1 : -1);
//			}
//
//			if (minOverlap > maxOverlap)
//			{
//				maxOverlap = minOverlap;
//				bestNormal = n;
//				bestIdxA = ca.first;
//				bestIdxB = cb.first;
//			}
//		}
//	}
//
//	if (!hit) return false;
//#pragma endregion
//
//#pragma region ‡B ‰Ÿ‚µo‚µ
//	if (NeedPush(a, b))
//	{
//		ApplyPush(a, b, bestNormal, maxOverlap);
//
//		// Ú’n”»’è
//		if (bestNormal.y > 0.5f)
//		{
//			a->CallOnGrounded();
//			b->CallOnGrounded();
//		}
//	}
//#pragma endregion
//
//	return true;
//}
#pragma endregion
bool CollisionManager::VoxelToVoxel(VoxelCollider* a, VoxelCollider* b)
{
#pragma region ‡@ rough AABB ”»’è
	//Vector3 aPos = a->GetPos();
	//Vector3 bPos = b->GetPos();

	//Vector3 aHalf = a->GetRoughSize() * 0.5f;
	//Vector3 bHalf = b->GetRoughSize() * 0.5f;

	//if (fabs(aPos.x - bPos.x) > aHalf.x + bHalf.x) return false;
	//if (fabs(aPos.y - bPos.y) > aHalf.y + bHalf.y) return false;
	//if (fabs(aPos.z - bPos.z) > aHalf.z + bHalf.z) return false;
#pragma endregion

#pragma region ‡A ƒZƒ‹’PˆÊi‘¦ returnj
//	const float halfA = a->GetCellSize() * 0.5f;
//	const float halfB = b->GetCellSize() * 0.5f;
//
//	for (const auto& ca : a->GetCellWorldPoss())
//	{
//		Vector3 aMin = ca.second - Vector3(halfA);
//		Vector3 aMax = ca.second + Vector3(halfA);
//
//		for (const auto& cb : b->GetCellWorldPoss())
//		{
//			Vector3 bMin = cb.second - Vector3(halfB);
//			Vector3 bMax = cb.second + Vector3(halfB);
//
//			// AABB vs AABB
//			if (aMax.x < bMin.x || aMin.x > bMax.x) continue;
//			if (aMax.y < bMin.y || aMin.y > bMax.y) continue;
//			if (aMax.z < bMin.z || aMin.z > bMax.z) continue;
//
//			// 1ƒZƒ‹‚Å‚à“–‚½‚Á‚½‚ç‘¦ true
//			return true;
//		}
//	}
#pragma endregion

	return false;
}

bool CollisionManager::LineToSphere(LineCollider* line, SphereCollider* sphere)
{
#pragma region •K—vî•ñ‚ğæ“¾
	// lineiü•ªj`````````````````````
	Vector3 dir = line->GetStartPos() - line->GetEndPos();
	Vector3 dirN = dir.Normalized(); 
	// ```````````````````````````
	// spherei‹…‘Ìj```````````
	Vector3 spherePos = sphere->GetPos();
	float radius = sphere->GetRadius();
	// ``````````````````
#pragma endregion

#pragma region Õ“Ë”»’èiSphere’†S‚©‚çü•ªÅ‹ß“_‚Ü‚Å‚Ì‹——£j
	Vector3 cp = line->ClosestPoint(spherePos);
	Vector3 diff = spherePos - cp;
	float distSq = diff.LengthSq();

	if (distSq > radius * radius) { return false; }
#pragma endregion

#pragma region Õ“ËŠm’èF•K—v‚É‰‚¶‚Ä‰Ÿ‚µo‚µ
	if (NeedPush(line, sphere)) {

		// ”»’è—pƒ‰ƒ€ƒ_ŠÖ”
		auto juged = [&](void)->bool {
			Vector3 sPos = sphere->GetPos();
			Vector3 lineClosePos = line->ClosestPoint(sPos);
			if ((sPos - lineClosePos).LengthSq() <= radius * radius) { return true; }
			return false;
			};

		// 1‰ñ‚Ì‰Ÿ‚µo‚·—Ê
		const float onePush = 5.0f;
		
		// ‰Ÿ‚µo‚µ‚ÌÅI“I‚ÈƒxƒNƒgƒ‹
		const Vector3 pushVec = line->GetDirection().Normalized() * onePush;

		// “–‚½‚ç‚È‚­‚È‚é‚Ü‚Å×‚©‚­‰Ÿ‚µo‚µ‘±‚¯‚éi–³ŒÀƒ‹[ƒv‘Îô‚ÅãŒÀ‚ğİ’è‚µ‚Ä‚¢‚éj
		for (unsigned char i = 0; i < 50; i++) {
			line->SetTransformPosAdd(pushVec);
			if (juged() == false) { break; }
		}
	}
#pragma endregion

	return true;
}

bool CollisionManager::LineToCapsule(LineCollider* line, CapsuleCollider* capsule)
{
#pragma region •K—vî•ñ‚ğæ“¾
	// lineiü•ªj``````````````
	const Vector3 P = line->GetStartPos();
	const Vector3 Q = line->GetEndPos();
	// ````````````````````

	// capsuleiƒJƒvƒZƒ‹j```````````
	const Vector3 A = capsule->GetStartPos();
	const Vector3 B = capsule->GetEndPos();
	const float radius = capsule->GetRadius();
	//`````````````````````

	// Å‹ß“_iCapsule‘¤j
	Vector3 capClosest = capsule->ClosestPoint(P);

	// Å‹ß“_iLine‘¤j 
	Vector3 lineClosest = line->ClosestPoint(capClosest);
#pragma endregion

#pragma region Õ“Ë”»’èij
	if ((capClosest - lineClosest).LengthSq() > radius * radius) { return false; }
#pragma endregion

#pragma region Õ“ËŠm’èF•K—v‚É‰‚¶‚Ä‰Ÿ‚µo‚µ
	if (NeedPush(line, capsule)) {
		// Õ“Ë”»’èƒ‰ƒ€ƒ_ŠÖ”
		auto juged = [&]() -> bool {
			Vector3 capC = capsule->ClosestPoint(line->GetPos());
			Vector3 lineC = line->ClosestPoint(capC);
			return (capC - lineC).LengthSq() <= radius * radius;
			};

		// ‚P‰ñ‚Å‰Ÿ‚µo‚·—Ê
		const float step = 5.0f;

		// ÅI“I‚É‰Ÿ‚µo‚·ƒxƒNƒgƒ‹
		Vector3 pushVec = line->GetDirection().Normalized() * step;

		// “–‚½‚ç‚È‚­‚È‚é‚Ü‚Å×‚©‚­‰Ÿ‚µo‚·i–³ŒÀƒ‹[ƒv‘Îô‚ÅãŒÀ‚ğİ’è‚µ‚Ä‚¨‚­j
		for (int i = 0; i < 50; i++) {
			line->SetTransformPosAdd(pushVec);
			if (juged() == false) { break; }
		}
	}
#pragma endregion

	return true;
}
#pragma region ‹ŒLineToBox
//bool CollisionManager::LineToBox(LineCollider* line, BoxCollider* box)
//{
//#pragma region •K—vî•ñ‚ğæ“¾
//	// lineiü•ªj````````````````````
//	Vector3 dir = line->GetStartPos() - line->GetEndPos();
//	Vector3 dirN = dir.Normalized();
//	// ``````````````````````````
//
//	// boxiƒ{ƒbƒNƒXj``````````
//	Vector3 boxPos = box->GetPos();
//	Vector3 half = box->GetSize() * 0.5f;
//
//	// AABB ‚ÌÅ¬/Å‘å
//	Vector3 bmin = boxPos - half;
//	Vector3 bmax = boxPos + half;
//	//``````````````````
//#pragma endregion
//
//#pragma region Õ“Ë”»’èij
//	// ü•ªÅ‹ß“_
//	Vector3 cp = line->ClosestPoint(boxPos);
//
//	// Å‹ß“_‚ª AABB ‚Ì’†‚É“ü‚Á‚Ä‚¢‚é‚©H
//	bool insideX = (cp.x >= bmin.x && cp.x <= bmax.x);
//	bool insideY = (cp.y >= bmin.y && cp.y <= bmax.y);
//	bool insideZ = (cp.z >= bmin.z && cp.z <= bmax.z);
//
//	if (!(insideX && insideY && insideZ)) { return false; }
//
//	Vector3 diff = boxPos - cp;
//
//	float overlapX = half.x - abs(diff.x);
//	float overlapY = half.y - abs(diff.y);
//	float overlapZ = half.z - abs(diff.z);
//
//	if (overlapX <= 0 || overlapY <= 0 || overlapZ <= 0) { return false; }
//#pragma endregion
//
//#pragma region Õ“ËŠm’èF•K—v‚É‰‚¶‚Ä‰Ÿ‚µo‚µ
//	if (NeedPush(line, box)) {
//		// Õ“Ë”»’èƒ‰ƒ€ƒ_ŠÖ”
//		auto juged = [&]() -> bool {
//			// ü•ªÅ‹ß“_
//			Vector3 cp = line->ClosestPoint(boxPos);
//
//			// Å‹ß“_‚ª AABB ‚Ì’†‚É“ü‚Á‚Ä‚¢‚é‚©H
//			bool insideX = (cp.x >= bmin.x && cp.x <= bmax.x);
//			bool insideY = (cp.y >= bmin.y && cp.y <= bmax.y);
//			bool insideZ = (cp.z >= bmin.z && cp.z <= bmax.z);
//
//			if (!(insideX && insideY && insideZ)) { return false; }
//
//			Vector3 diff = boxPos - cp;
//
//			float overlapX = half.x - abs(diff.x);
//			float overlapY = half.y - abs(diff.y);
//			float overlapZ = half.z - abs(diff.z);
//
//			if (overlapX <= 0 || overlapY <= 0 || overlapZ <= 0) { return false; }
//
//			return true;
//			};
//
//		// ‚P‰ñ‚Å‰Ÿ‚µo‚·—Ê
//		const float step = 5.0f;
//
//		// ÅI“I‚É‰Ÿ‚µo‚·ƒxƒNƒgƒ‹
//		Vector3 pushVec = line->GetDirection().Normalized() * step;
//
//		// “–‚½‚ç‚È‚­‚È‚é‚Ü‚Å×‚©‚­‰Ÿ‚µo‚·i–³ŒÀƒ‹[ƒv‘Îô‚ÅãŒÀ‚ğİ’è‚µ‚Ä‚¨‚­j
//		for (int i = 0; i < 50; i++) {
//			line->SetTransformPosAdd(pushVec);
//			if (juged() == false) { break; }
//		}
//
//		if (pushVec.y > 0.5f) { line->CallOnGrounded(); }
//	}
//#pragma endregion
//
//	return true;
//}
#pragma endregion
bool CollisionManager::LineToBox(LineCollider* line, BoxCollider* box)
{
	// -----------------------------
	// ‰Ÿ‚µo‚µ•ûŒüiŒÅ’èj
	// -----------------------------
	Vector3 pushDir = line->GetDirection().Normalized();

	// -----------------------------
	// Box î•ñ
	// -----------------------------
	Vector3 boxPos = box->GetPos();
	Vector3 half = box->GetSize() * 0.5f;

	Vector3 bmin = boxPos - half;
	Vector3 bmax = boxPos + half;

	// -----------------------------
	// Rough ”»’è
	// -----------------------------
	Vector3 cp = line->ClosestPoint(boxPos);

	if (cp.x < bmin.x || cp.x > bmax.x ||
		cp.y < bmin.y || cp.y > bmax.y ||
		cp.z < bmin.z || cp.z > bmax.z)
	{
		return false;
	}

	// -----------------------------
	// Ú×”»’è + Å[“_Œˆ’è
	// -----------------------------
	Vector3 hitPoint = cp;
	Vector3 local = hitPoint - boxPos;

	Vector3 overlap(
		half.x - fabs(local.x),
		half.y - fabs(local.y),
		half.z - fabs(local.z)
	);

	if (overlap.x <= 0 || overlap.y <= 0 || overlap.z <= 0)
		return false;

	// -----------------------------
	// ‰Ÿ‚µo‚µ
	// -----------------------------
	if (NeedPush(line, box))
	{
		// ‰Ÿ‚µo‚µ‹——£‚ğ‰Ÿ‚µo‚µ•ûŒü¬•ª‚ÅŒˆ’è
		float pushDist =
			fabs(pushDir.x) * overlap.x +
			fabs(pushDir.y) * overlap.y +
			fabs(pushDir.z) * overlap.z;

		// ˆÀ‘Sƒ}[ƒWƒ“
		pushDist += 0.001f;

		Vector3 pushVec = pushDir * pushDist;
		line->SetTransformPosAdd(pushVec);

		if (pushDir.y > 0.5f) { line->CallOnGrounded(); }
	}

	return true;
}

bool CollisionManager::LineToModel(LineCollider* line, ModelCollider* model)
{
	return false;
}

bool CollisionManager::LineToVoxel(LineCollider* line, VoxelCollider* voxel)
{
	// ”O‚Ì‚½‚ßƒqƒbƒgƒZƒ‹î•ñ‚ğƒNƒŠƒA
	voxel->ClearHitCellIdxs();

	// ‰Ÿ‚µo‚µ•ûŒüiŒÅ’èj
	Vector3 pushDir = line->GetDirection().Normalized();

#pragma region ‘å‚Ü‚©‚ÈƒTƒCƒY‚Å”»’è
	Vector3 voxelPos = voxel->GetPos();
	Vector3 roughHalf = voxel->GetRoughSize() * 0.5f;

	Vector3 bmin = voxelPos - roughHalf;
	Vector3 bmax = voxelPos + roughHalf;

	Vector3 cp = line->ClosestPoint(voxelPos);

	if (cp.x < bmin.x || cp.x > bmax.x ||
		cp.y < bmin.y || cp.y > bmax.y ||
		cp.z < bmin.z || cp.z > bmax.z)
	{
		return false;
	}
#pragma endregion

#pragma region ×‚©‚­ƒZƒ‹’PˆÊ‚Å”»’è + ÅVÕ“Ë’Tõ
	const float cellSize = voxel->GetCellSize();
	Vector3 halfCell(cellSize * 0.5f);

	bool hit = false;

	float bestScore = -FLT_MAX;
	Vector3 bestHitPoint;
	Vector3 bestCellCenter;

	// ‘S‚Ä‚ÌƒZƒ‹‚ğƒ`ƒFƒbƒN
	for (const auto& kv : voxel->GetCellWorldPoss())
	{

		Vector3 minCell = kv.second - halfCell;
		Vector3 maxCell = kv.second + halfCell;

		Vector3 hitPoint = line->ClosestPoint(kv.second);

		if (hitPoint.x < minCell.x || hitPoint.x > maxCell.x ||
			hitPoint.y < minCell.y || hitPoint.y > maxCell.y ||
			hitPoint.z < minCell.z || hitPoint.z > maxCell.z)
		{
			continue;
		}

		// AABB overlap Šm”F
		Vector3 overlap = halfCell - (kv.second - hitPoint).Abs();
		if (overlap.x <= 0 || overlap.y <= 0 || overlap.z <= 0)
		{
			continue;
		}

		hit = true;
		voxel->AddHitCellIdx(kv.first);

		// ‰Ÿ‚µo‚µ•ûŒü‚ÉÅ‚à[‚¢“_‚ğ‘I‚Ô
		float score = hitPoint.Dot(pushDir);
		if (score > bestScore)
		{
			bestScore = score;
			bestHitPoint = hitPoint;
			bestCellCenter = kv.second;
		}
	}

	// “–‚½‚Á‚Ä‚¢‚È‚©‚Á‚½‚çI—¹
	if (!hit) { return false; }
#pragma endregion

#pragma region •K—v‚É‰‚¶‚Ä‰Ÿ‚µo‚µˆ—
	if (NeedPush(line, voxel) && pushDir.Dot(line->GetTransform().Velocity().Normalized()) <= 0.0f) {
		// ƒZƒ‹–Ê‚Ü‚Å‚Ì‹——£‚ğŒvZ
		Vector3 toCell = bestHitPoint - bestCellCenter;

		float pushDist =
			(halfCell.x * fabs(pushDir.x) +
				halfCell.y * fabs(pushDir.y) +
				halfCell.z * fabs(pushDir.z)) -
			toCell.Dot(pushDir);

		// ˆÀ‘Sƒ}[ƒWƒ“
		pushDist += 0.001f;

		Vector3 pushVec = pushDir * pushDist;
		line->SetTransformPosAdd(pushVec);

		if (pushDir.y > 0.5f) { line->CallOnGrounded(); }
	}
#pragma endregion

	return true;
}

bool CollisionManager::SphereToCapsule(SphereCollider* sphere, CapsuleCollider* capsule)
{
#pragma region •K—vî•ñ‚ğæ“¾
	// spherei‹…‘Ìj`````````````````
	// À•W
	const Vector3 C = sphere->GetPos();
	// ”¼Œa
	const float   rS = sphere->GetRadius();
	// ```````````````````

	// capsuleiƒJƒvƒZƒ‹j```````````````
	// ü•ª‚Ì n“_/I“_ À•W
	const Vector3 A = capsule->GetStartPos();
	const Vector3 B = capsule->GetEndPos();
	// ”¼Œa
	const float   rC = capsule->GetRadius();
	// ```````````````````
#pragma endregion

#pragma region Õ“Ë”»’èispherei‹…‘Ìj‚Ì’†SÀ•W‚©‚çcapsuleiƒJƒvƒZƒ‹jü•ªã‚É‚¨‚¯‚éÅ‹ß“_‚Ü‚Å‚Ì‹——£‚ğ‚Í‚©‚Á‚Ä –¢Õ“Ë‚È‚çI—¹j
	// spherei‹…‘Ìj‚Ì’†SÀ•W‚©‚çAcapsuleiƒJƒvƒZƒ‹jü•ªã‚Åˆê”Ô‹ß‚¢“_‚ğ‹‚ß‚é``
	Vector3 AB = B - A;
	Vector3 AC = C - A;
	float abLenSq = AB.LengthSq();

	float t = 0.0f;
	if (abLenSq > 1e-6f) {
		t = AC.Dot(AB) / abLenSq;
		t = std::clamp(t, 0.0f, 1.0f);
	}
	Vector3 Q = A + AB * t;
	//````````````````````````````````````````

	// spherei‹…‘Ìj‚Ì’†SÀ•W‚©‚çA‹‚ß‚½capsuleiƒJƒvƒZƒ‹jü•ªã‚É‚¨‚¯‚éÅ‹ß“_‚Ü‚Å‚Ì‹——£‚ğ‚Í‚©‚Á‚ÄA‚¨Œİ‚¢‚Ì”¼Œa‚Ì‡Œv‚Æ”ä‚×‚é``
	
	// ‚Q“_ŠÔ‚ÌƒxƒNƒgƒ‹
	Vector3 normal = C - Q;

	// ‹——£‚Ì‚QæiŒvZ—ÊŒyŒ¸‚Ì‚½‚ß‚Qæ‚Åæ“¾jAŒã‚Ù‚Çg‚¤‰Â”\«‚ª‚ ‚é‚Ì‚Åƒ[ƒJƒ‹•Ï”‚É•Û‚µ‚Ä‚¨‚­
	float distSq = normal.LengthSq();

	// ‚¨Œİ‚¢‚Ì”¼Œa‚Ì‡Œv
	float radiusSum = rS + rC;

	// ‹——£‚Ì‚Qæ‚Æ‚¨Œİ‚¢‚Ì”¼Œa‚Ì‡Œv‚Ì‚Qæ‚ğ”ä‚×‚Ä”»’èi–¢Õ“Ë‚È‚çI—¹j
	if (distSq >= radiusSum * radiusSum) { return false; }

	//````````````````````````````````````````````````````````````````
#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	// ‰Ÿ‚µo‚µ‚ª•K—v‚©‚Ç‚¤‚©
	if (NeedPush(sphere, capsule))
	{
		// Õ“Ë”»’èæ“¾‚µ‚½dispSq‚ğg‚Á‚ÄAÀÛ‚Ì‹——£‚ğZo‚·‚é
		float dist = std::sqrtf(distSq);

		if (dist < 1e-6f) {
			// Š®‘Sˆê’v‚µ‚Ä‚¢‚½‚ç“K“–‚È•ûŒüiˆÚ“®•ûŒü‚Ì‹t•ûŒüj‚ğ—^‚¦‚é
			normal = -sphere->GetTransform().Velocity();
			dist = 0.0f;
		}

		// ‚ß‚è‚İ—Ê
		float overlap = radiusSum - dist;

		// ‰Ÿ‚µo‚µˆ—
		ApplyPush(sphere, capsule, normal.Normalized(), overlap);
	}
#pragma endregion

	// “–‚½‚Á‚½
	return true;
}

bool CollisionManager::SphereToBox(SphereCollider* sphere, BoxCollider* box)
{
	Vector3 c = sphere->GetPos();
	float r = sphere->GetRadius();

	Vector3 boxPos = box->GetPos();
	Vector3 half = box->GetSize() * 0.5f;

	// Å‹ß“_
	Vector3 nearest;
	nearest.x = std::clamp(c.x, boxPos.x - half.x, boxPos.x + half.x);
	nearest.y = std::clamp(c.y, boxPos.y - half.y, boxPos.y + half.y);
	nearest.z = std::clamp(c.z, boxPos.z - half.z, boxPos.z + half.z);

	Vector3 normal = c - nearest;
	float distSq = normal.LengthSq();

	if (distSq > r * r) { return false; }

	//------------------------------------------
	// ‰Ÿ‚µo‚µ
	//------------------------------------------
	if (NeedPush(sphere, box))
	{
		float dist = sqrtf(distSq);

		Vector3 pushNormal;

		if (dist > 0.0001f)
		{
			pushNormal = normal / dist;
		}
		else
		{
			// ˆê’v ¨ ‹…‚ª‚¿‚å‚¤‚Ç–Ê‚Éæ‚Á‚Ä‚¢‚é
			// –Ê–@ü‚ğŒvZ‚·‚é
			Vector3 diff = c - boxPos;

			float dx = fabs(diff.x) - half.x;
			float dy = fabs(diff.y) - half.y;
			float dz = fabs(diff.z) - half.z;

			// 1”Ô‚ß‚è‚ñ‚Å‚¢‚é•ûŒü–Ê–@ü
			if (dx >= dy && dx >= dz)
				pushNormal = Vector3((diff.x > 0 ? 1 : -1), 0, 0);
			else if (dy >= dx && dy >= dz)
				pushNormal = Vector3(0, (diff.y > 0 ? 1 : -1), 0);
			else
				pushNormal = Vector3(0, 0, (diff.z > 0 ? 1 : -1));
		}

		float overlap = r - dist;
		if (overlap < 0) overlap = 0;

		ApplyPush(sphere, box, pushNormal, overlap);

		// °Ú’n”»’è
		if (pushNormal.y > 0.5f)
			sphere->CallOnGrounded();
	}

	return true;
}

bool CollisionManager::SphereToModel(SphereCollider* sphere, ModelCollider* model)
{
	return false;
}

bool CollisionManager::SphereToVoxel(SphereCollider* sphere, VoxelCollider* voxel)
{
	// Õ“ËƒZƒ‹ŒQî•ñ‚ğƒŠƒZƒbƒg
	voxel->ClearHitCellIdxs();

#pragma region •K—vî•ñ‚ğæ“¾
	Vector3 spherePos = sphere->GetPos();
	float radius = sphere->GetRadius();
	Vector3 voxelPos = voxel->GetPos();
	Vector3 roughHalf = voxel->GetRoughSize() * 0.5f;
#pragma endregion

#pragma region Õ“Ë”»’èij
	// ‡@ ‘e”»’èFSphere vs rough AABB````````````````````````````
	Vector3 nearest;
	nearest.x = std::clamp(spherePos.x, voxelPos.x - roughHalf.x, voxelPos.x + roughHalf.x);
	nearest.y = std::clamp(spherePos.y, voxelPos.y - roughHalf.y, voxelPos.y + roughHalf.y);
	nearest.z = std::clamp(spherePos.z, voxelPos.z - roughHalf.z, voxelPos.z + roughHalf.z);

	Vector3 dd = spherePos - nearest;
	if (dd.LengthSq() > radius * radius) { return false; }
	//````````````````````````````````````````````
	
	// ‡A ×”»’èFƒZƒ‹’PˆÊƒ`ƒFƒbƒN``````````````````````
	const float cellSize = voxel->GetCellSize();

	bool hit = false;

	// Å‚à‚ß‚è‚ñ‚¾ƒZƒ‹i‰Ÿ‚µo‚µ‚Ég‚¤j
	float overlap = 0.0f;
	int   deepestCellIdx = -1;
	Vector3 deepestCellNormal = -sphere->GetTransform().Velocity().Normalized();
	Vector3 deepestCellCenter;

	for (const std::pair<int, Vector3>& kv : voxel->GetCellWorldPoss()) {
		int idx = kv.first;
		Vector3 center = kv.second;

		Vector3 cmin = center - Vector3(cellSize * 0.5f);
		Vector3 cmax = center + Vector3(cellSize * 0.5f);

		// Å‹ß“_
		Vector3 nearP = {
			std::clamp(spherePos.x, cmin.x, cmax.x),
			std::clamp(spherePos.y, cmin.y, cmax.y),
			std::clamp(spherePos.z, cmin.z, cmax.z)
		};

		Vector3 diff = spherePos - nearP;
		float distSq = diff.LengthSq();

		if (distSq <= radius * radius) {
			hit = true;
			voxel->AddHitCellIdx(idx);

			float dist = sqrtf(distSq);
			float workOverlap = radius - dist;

			if (workOverlap > overlap) {
				overlap = workOverlap;
				deepestCellIdx = idx;
				deepestCellCenter = center;

				if (dist > 0.0001f) { deepestCellNormal = diff / dist; }
				else { deepestCellNormal = -sphere->GetTransform().Velocity().Normalized(); }
			}
		}
	}

	if (hit == false) { return false; }
	// ````````````````````````````````````

#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	// ‰Ÿ‚µo‚µ‚ª•K—v‚©
	if (NeedPush(sphere, voxel)) {
		// Å‚à[‚¢ƒZƒ‹‚¾‚¯g‚¤
		ApplyPush(sphere, voxel, deepestCellNormal, overlap);

		// Ú’n”»’èiY•ûŒüj
		if (deepestCellNormal.y > 0.5f) { sphere->CallOnGrounded(); }
	}
#pragma endregion

	return true;
}

bool CollisionManager::CapsuleToBox(CapsuleCollider* capsule, BoxCollider* box)
{
#pragma region •K—vî•ñ‚Ìæ“¾
	// capsuleiƒJƒvƒZƒ‹j```````````
	// ü•ª‚Ì n“_/I“_ À•W
	const Vector3 A = capsule->GetStartPos(), B = capsule->GetEndPos();
	// ”¼Œa
	const float   r = capsule->GetRadius();
	// ````````````````````

	// boxiƒ{ƒbƒNƒXj`````````````
	// À•W
	const Vector3 boxPos = box->GetPos();
	// ‘å‚«‚³i”¼•ªj
	const Vector3 half = box->GetSize() / 2;
	// ````````````````````
#pragma endregion

#pragma region Õ“Ë”»’èij
	// Step1FcapsuleiƒJƒvƒZƒ‹jü•ªã‚Åboxiƒ{ƒbƒNƒXj‚ÉÅ‚à‹ß‚¢“_‚ğ‹‚ß‚é
	Vector3 AB = B - A;
	float abLenSq = AB.LengthSq();
	float t = 0.0f;

	if (abLenSq > 1e-6f) {
		// A¨B‚Ìü•ªã‚ÅABox’†S‚ª‚Ç‚ÌˆÊ’u‚É“Š‰e‚³‚ê‚é‚©
		t = (boxPos - A).Dot(AB) / abLenSq;
		t = std::clamp(t, 0.0f, 1.0f);
	}

	// Å‹ß“_iƒJƒvƒZƒ‹ü•ªãj
	Vector3 P = A + AB * t;

	// Step2FP ‚Æ Box ‚ÌÅ‚à‹ß‚¢“_‚ğ‹‚ß‚éiSphereToBox ‚Æ“¯‚¶Œ´—j
	Vector3 nearest;
	nearest.x = std::clamp(P.x, boxPos.x - half.x, boxPos.x + half.x);
	nearest.y = std::clamp(P.y, boxPos.y - half.y, boxPos.y + half.y);
	nearest.z = std::clamp(P.z, boxPos.z - half.z, boxPos.z + half.z);

	Vector3 normal = P - nearest;
	float distSq = normal.LengthSq();

	// –¢Õ“Ë
	if (distSq > r * r) { return false; }
#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	// ‰Ÿ‚µo‚µ‚ª•K—v‚©
	if (NeedPush(capsule, box)) {

		// Õ“Ë”»’èæ“¾‚µ‚½dispSq‚ğg‚Á‚ÄAÀÛ‚Ì‹——£‚ğZo‚·‚é
		float dist = sqrtf(distSq);

		if (dist <= 0.0001f) {
			// Š®‘Sˆê’v‚µ‚Ä‚¢‚½‚ç“K“–‚È•ûŒüiˆÚ“®•ûŒü‚Ì‹t•ûŒüj‚ğ—^‚¦‚é
			normal = -capsule->GetTransform().Velocity();
			dist = 0.0f;
		}

		// ‚ß‚è‚ñ‚¾—Ê
		float overlap = r - dist;

		// ‰Ÿ‚µo‚µˆ—
		ApplyPush(capsule, box, normal.Normalized(), overlap);
	}
#pragma endregion

	return true;
}

bool CollisionManager::CasuleToModel(CapsuleCollider* capsule, ModelCollider* model)
{
	return false;
}

bool CollisionManager::CapsuleToVoxel(CapsuleCollider* capsule, VoxelCollider* voxel)
{
	voxel->ClearHitCellIdxs();

#pragma region •K—vî•ñ‚Ìæ“¾
	Vector3 capA = capsule->GetStartPos();
	Vector3 capB = capsule->GetEndPos();
	float radius = capsule->GetRadius();
	Vector3 voxelPos = voxel->GetPos();
	Vector3 roughHalf = voxel->GetRoughSize() * 0.5f;
#pragma endregion

#pragma region Õ“Ë”»’èij
	// ‘e”»’èFCapsule ‚Ì AABB vs RoughAABB``````````````
	Vector3 capMin = {
		(std::min)(capA.x, capB.x) - radius,
		(std::min)(capA.y, capB.y) - radius,
		(std::min)(capA.z, capB.z) - radius
	};
	Vector3 capMax = {
		(std::max)(capA.x, capB.x) + radius,
		(std::max)(capA.y, capB.y) + radius,
		(std::max)(capA.z, capB.z) + radius
	};

	Vector3 voxMin = voxelPos - roughHalf;
	Vector3 voxMax = voxelPos + roughHalf;

	if (capMax.x < voxMin.x || capMin.x > voxMax.x) { return false; }
	if (capMax.y < voxMin.y || capMin.y > voxMax.y) { return false; }
	if (capMax.z < voxMin.z || capMin.z > voxMax.z) { return false; }
	// ``````````````````````````````````

	// ×”»’èFƒZƒ‹’PˆÊƒ`ƒFƒbƒN`````````````````````````
	const float cellSize = voxel->GetCellSize();

	bool hit = false;
	float overlap = -1.0f;
	int deepestCellIdx = -1;

	Vector3 deepestNormal = Vector3(0, 1, 0);
	Vector3 deepestCenter;

	for (auto& kv : voxel->GetCellWorldPoss())
	{
		int idx = kv.first;
		Vector3 center = kv.second;

		Vector3 cmin = center - Vector3(cellSize * 0.5f);
		Vector3 cmax = center + Vector3(cellSize * 0.5f);

		//--------------------------------------------------
		// Capsule vs AABBi1ƒZƒ‹j
		//--------------------------------------------------

		// Step1FƒJƒvƒZƒ‹ü•ª‚Æ AABB ‚ÌÅ‹ß“_‚ğæ‚é
		Vector3 Q;  // Å‹ß“_iAABB“àj
		// ‚Ü‚¸ AABB ‚É‘Î‚µü•ª‚ÌÅ‹ß“_‚ğ‹‚ß‚é
		// ü•ª param t
		Vector3 AB = capB - capA;
		float lenSq = AB.LengthSq();
		float t = 0.0f;

		if (lenSq > 1e-6f) {
			// AABB ‚Ì’†S•ûŒü‚Ì‘ã•\“_‚Æ‚µ‚Ä center ‚ğg‚¤•û®i‚‘¬j
			t = (center - capA).Dot(AB) / lenSq;
			t = std::clamp(t, 0.0f, 1.0f);
		}

		Vector3 closestOnLine = capA + AB * t;

		// AABB ‚Ö‚Ì clamp
		Q.x = std::clamp(closestOnLine.x, cmin.x, cmax.x);
		Q.y = std::clamp(closestOnLine.y, cmin.y, cmax.y);
		Q.z = std::clamp(closestOnLine.z, cmin.z, cmax.z);

		Vector3 diff = (Q - Q); // dummy init
		diff = capsule->ClosestPoint(Q) - Q;

		float distSq = diff.LengthSq();
		if (distSq <= radius * radius) {
			hit = true;
			voxel->AddHitCellIdx(idx);

			float dist = sqrtf(distSq);
			float workOverlap = radius - dist;

			if (workOverlap > overlap) {
				overlap = workOverlap;
				deepestCellIdx = idx;
				deepestCenter = center;

				if (dist > 0.0001f) { deepestNormal = (capsule->ClosestPoint(Q) - Q).Normalized(); }
				else {
					Vector3 vel = capsule->GetTransform().Velocity();
					deepestNormal = (vel.LengthSq() > 0.0001f)
						? -vel.Normalized()
						: Vector3(0, 1, 0);
				}
			}
		}
	}

	if (!hit) { return false; }
	// ``````````````````````````````````
#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	if (NeedPush(capsule, voxel)) {
		// ‰Ÿ‚µo‚µˆ—
		ApplyPush(capsule, voxel, deepestNormal, overlap);

		// Ú’n”»’è
		if (deepestNormal.y > 0.5f) { capsule->CallOnGrounded(); }
	}
#pragma endregion

	return true;
}

bool CollisionManager::BoxToModel(BoxCollider* box, ModelCollider* model)
{
	return false;
}

bool CollisionManager::BoxToVoxel(BoxCollider* box, VoxelCollider* voxel)
{
#pragma region •K—vî•ñ‚ğæ“¾
	Vector3 normal = box->GetPos() - voxel->GetPos();
	Vector3 halfSizeSum = (box->GetSize() + voxel->GetRoughSize()) * 0.5f;
#pragma endregion

#pragma region Õ“Ë”»’èij
	if (abs(normal.x) > halfSizeSum.x) { return false; }
	if (abs(normal.y) > halfSizeSum.y) { return false; }
	if (abs(normal.z) > halfSizeSum.z) { return false; }
#pragma endregion

#pragma region Õ“ËŠm’èF•K—v‚È‚ç‰Ÿ‚µo‚µ
	if (NeedPush(box, voxel)) {

		Vector3 overlapNorm = halfSizeSum - normal.Abs();

		ApplyPush(box, voxel, overlapNorm.MinElement() * (normal / normal.Abs()));
	}
#pragma endregion

	return true;
}

bool CollisionManager::ModelToVoxel(ModelCollider* model, VoxelCollider* voxel)
{
	return false;
}