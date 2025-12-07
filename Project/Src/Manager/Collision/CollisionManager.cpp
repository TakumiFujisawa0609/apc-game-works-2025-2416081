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
	case TAG::ENEMY:
	case TAG::BOSS:
	case TAG::GOLEM_ATTACK_FALL:
	case TAG::GOLEM_ATTACK_PSYCHOROCK:
	case TAG::GOLEM_ATTACK_STONE:
		enemyColliders_.emplace_back(collider);
		break;

		// ƒXƒe[ƒWŒn
	case TAG::STAGE:
		stageColliders_.emplace_back(collider);
		break;

		// ‚»‚êˆÈŠO
	case TAG::GOLEM_ATTACK_WALL:
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

bool CollisionManager::IsStageCollision(const Vector3& pos, float radius)
{
	for (ColliderBase*& collider : stageColliders_) {

		if (auto voxel = dynamic_cast<VoxelCollider*>(collider)) {
			Vector3 nearest = {};

			Vector3 voxelRoughPos = voxel->GetPos();
			Vector3 voxelRoughHalfSize = voxel->GetRoughSize();

			nearest = {
				std::clamp(pos.x, voxelRoughPos.x - voxelRoughHalfSize.x, voxelRoughPos.x + voxelRoughHalfSize.x),
				std::clamp(pos.y, voxelRoughPos.y - voxelRoughHalfSize.y, voxelRoughPos.y + voxelRoughHalfSize.y),
				std::clamp(pos.z, voxelRoughPos.z - voxelRoughHalfSize.z, voxelRoughPos.z + voxelRoughHalfSize.z)
			};

			if ((pos - nearest).LengthSq() >= radius * radius) { continue; }

			float cellHalfSize = voxel->GetCellSize() * 0.5f;

			for (std::pair<const int, Vector3>& cellPos : voxel->GetCellWorldPoss()) {
				nearest = {
					std::clamp(pos.x, cellPos.second.x - cellHalfSize, cellPos.second.x + cellHalfSize),
					std::clamp(pos.z, cellPos.second.z - cellHalfSize, cellPos.second.z + cellHalfSize),
					std::clamp(pos.y, cellPos.second.y - cellHalfSize, cellPos.second.y + cellHalfSize)
				};

				if ((pos - nearest).LengthSq() < radius * radius) { return true; }
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
	signed int size = s.size();

	for (int a = 0; a < (size - 1); a++) {
		if (!s[a]) { continue; }
		if (!s[a]->GetJudge()) { continue; }

		for (int b = a + 1; b < size; b++) {
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
#pragma region •K—vî•ñ‚ğæ“¾
	// spherei‹…‘Ìj```````````
	// À•W
	Vector3 spherePos = sphere->GetPos();
	// ”¼Œa
	float radius = sphere->GetRadius();
	// ``````````````````
#pragma endregion

#pragma region Õ“Ë”»’èispherei‹…‘Ìj‚ÌÀ•W‚Æü•ªã‚ÌÅ‹ß“_‚Ì‹——£‚ğ‚Í‚©‚Á‚Ä –¢Õ“Ë‚È‚çI—¹j
	// spherei‹…‘Ìj‚ÌÀ•W‚©‚ç‚İ‚½ü•ªã‚É‚¨‚¯‚éÅ‹ß“_‚ğæ“¾‚µ‚Ä‚»‚±‚Æ‚ÌƒxƒNƒgƒ‹‚ğì¬
	Vector3 normal = spherePos - line->ClosestPoint(spherePos);

	// ƒxƒNƒgƒ‹‚©‚ç‹——£‚Ì‚Qæ‚ğŒvZ
	float distSq = normal.LengthSq();

	// ‹——£‚Ì‚Qæ‚Æspherei‹…‘Ìj‚Ì”¼Œa‚Ì‚Qæ‚ğ”ä‚×‚éB–¢Õ“Ë‚¾‚Á‚½‚çI—¹
	if (distSq > radius * radius) { return false; }
#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	// ‰Ÿ‚µo‚µ‚ª•K—v‚©‚Ç‚¤‚©
	if (NeedPush(line, sphere))
	{
		// Õ“Ë”»’èæ“¾‚µ‚½dispSq‚ğg‚Á‚ÄAÀÛ‚Ì‹——£‚ğZo‚·‚é
		float dist = sqrtf(distSq);

		// ‚ß‚è‚ñ‚¾—Ê
		float overlap = radius - dist;

		// –{—ˆ‚Ì–@ü•ûŒü
		Vector3 n = (dist > 0.0001f) ? normal / dist : Vector3(0, 1, 0);

		// ‰Ÿ‚µo‚µ•ûŒüiLine ‚ğ‰Ÿ‚µo‚·•ûŒüj
		Vector3 pushDir = line->GetDirection().Normalized();

		// –@ü‚ğ‰Ÿ‚µo‚µ•ûŒü‚É“Š‰e‚·‚é
		float dotValue = n.Dot(pushDir);

		// ‚±‚Ì dot ‚ª 0 ‚È‚ç‰Ÿ‚µo‚¹‚È‚¢iŠ®‘S‚É’¼Œğj
		if (fabs(dotValue) < 1e-5f) {
			return true; // ‚Ü‚½‚Í–@ü•ûŒü‚Å‰Ÿ‚µo‚· fallback ‚ğ‘‚¢‚Ä‚à—Ç‚¢
		}

		// “Š‰e—Ê‚É‚æ‚Á‚Ä“KØ‚È‰Ÿ‚µo‚µ—Ê‚ğŒˆ’è
		float finalOverlap = overlap * dotValue;

		// ¡‰ñ‚Íuline ‚Ì•ûv‚ğ‰Ÿ‚µo‚·
		if (line->GetDynamicFlg())
		{
			Vector3 pushVec = pushDir * finalOverlap;
			line->SetTransformPos(line->GetTransform().pos + pushVec);
		}
	}
#pragma endregion

	return true;
}

bool CollisionManager::LineToCapsule(LineCollider* line, CapsuleCollider* capsule)
{
#pragma region •K—vî•ñ‚ğæ“¾
	// lineiü•ªj`````````

	// ```````````````

	// capsuleiƒJƒvƒZƒ‹j````````

	//``````````````````
#pragma endregion

#pragma region Õ“Ë”»’èij

#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
#pragma endregion

	return true;
}

bool CollisionManager::LineToBox(LineCollider* line, BoxCollider* box)
{
#pragma region •K—vî•ñ‚ğæ“¾
	// boxiƒ{ƒbƒNƒXj````````````
	Vector3 boxPos = box->GetPos();
	Vector3 half = box->GetSize() / 2;
	//````````````````````
#pragma endregion

#pragma region Õ“Ë”»’èij

	// ü•ªã‚ÌÅ‹ß“_‚ğ‹‚ß‚é
	Vector3 closest = line->ClosestPoint(boxPos);
	// Box ‚É‘Î‚·‚éÅ‹ß“_‚ğ‹‚ß‚é
	Vector3 nearest;
	nearest.x = std::clamp(closest.x, boxPos.x - half.x, boxPos.x + half.x);
	nearest.y = std::clamp(closest.y, boxPos.y - half.y, boxPos.y + half.y);
	nearest.z = std::clamp(closest.z, boxPos.z - half.z, boxPos.z + half.z);

	// Å‹ß“_ŠÔƒxƒNƒgƒ‹
	Vector3 normal = closest - nearest;
	float distSq = normal.LengthSq();

	// Š®‘Sˆê’vid‚È‚è‹É¬j‰ñ”ğ
	if (distSq < 1e-8f) {
		normal = Vector3(0, 1, 0); // ‰¼‚Ì–@ü
		distSq = 0.0f;
	}

	// Õ“Ë”»’è
	if (distSq > 0.0f) {
		// Box ‚Í”¼Œa‚Å‚Í‚È‚­u“_‚Æ‹«ŠE‚Ì‹——£v‚Å‚ß‚è‚İ—Ê‚ğ‘ª‚é‚½‚ßA
		// ‚±‚±‚Å‚Í distSq > 0 (=–¢Õ“Ë) ‚ğ•Ô‚·B
		// ‚½‚¾‚µAclosest ‚ª Box ‚Ì“à•”‚É‚ ‚éê‡‚ÍÕ“Ë 100%
		// ¨ ‰º‚Ì‰Ÿ‚µo‚µŒvZ‚Éi‚Ş
		float dist = sqrtf(distSq);

		// Å‹ß“_‚ª Box “à•”‚È‚ç dist == 0 ‚É‚È‚éƒP[ƒX‚à‚ ‚é
		if (dist > 0.0f) {
			// Box ‚ÌŠO‘¤ ¨ –¢Õ“Ë
			return false;
		}
	}
#pragma endregion

#pragma region Õ“ËŠm’èF•K—v‚É‰‚¶‚Ä‰Ÿ‚µo‚µ
	// Õ“ËŠm’èiclosest ‚ª Box ‚Ì“à‘¤j
	float dist = sqrtf(distSq);
	// Š®‘S‚É‚ß‚è‚ñ‚Å‚é‚Æ‰Ÿ‚µo‚µ•s”\‚È‚Ì‚ÅÅ’á—Ê‚Ì‰Ÿ‚µo‚µ•â³
	float overlap = (dist > 0.0f) ? -dist : 0.01f;

	// ‰Ÿ‚µo‚µ‚ª•K—v‚©H
	if (NeedPush(line, box))
	{
		// –{—ˆ‚Ì–@ü•ûŒüi’nŒ`‘¤‚ª‚ß‚è‚ñ‚Å‚é•ûŒüj
		Vector3 n;
		if (dist > 0.0f) { n = normal / dist; }
		else { n = Vector3(0, 1, 0); }

		// Line ‘¤‚É‰Ÿ‚µo‚µ‚½‚¢•ûŒü
		Vector3 pushDir = line->GetDirection().Normalized();

		// –@ü‚ğ‰Ÿ‚µo‚µ•ûŒü‚É“Š‰e ¨ ³Šm‚É‰Ÿ‚µo‚¹‚é—Ê‚ª‹‚Ü‚é
		float dotValue = n.Dot(pushDir);

		// ‰Ÿ‚µo‚µƒ[ƒ‚È‚ç‚Ç‚¤‚µ‚æ‚¤‚à‚È‚¢i’¼Œğj
		if (fabs(dotValue) < 1e-5f) { return true; }

		float finalOverlap = overlap * dotValue;

		// lineiü•ªj‚ğ‰Ÿ‚µo‚·
		if (line->GetDynamicFlg())
		{
			Vector3 pushVec = pushDir * finalOverlap;
			line->SetTransformPos(line->GetTransform().pos + pushVec);
		}
	}
#pragma endregion

	return true;
}

bool CollisionManager::LineToModel(LineCollider* line, ModelCollider* model)
{
	return false;
}

bool CollisionManager::LineToVoxel(LineCollider* line, VoxelCollider* voxel)
{
	voxel->ClearHitCellIdxs();

#pragma region •K—vî•ñ‚ğæ“¾
	// lineiü•ªj````````````````````````````````````````
	Vector3 a = line->GetStartPos();
	Vector3 b = line->GetEndPos();
	Vector3 lineMin = Vector3((std::min)(a.x, b.x), (std::min)(a.y, b.y), (std::min)(a.z, b.z));
	Vector3 lineMax = Vector3((std::max)(a.x, b.x), (std::max)(a.y, b.y), (std::max)(a.z, b.z));
	// ``````````````````````````````````````````````

	// voxeliƒ{ƒNƒZƒ‹j````````````````
	Vector3 voxPos = voxel->GetPos();
	Vector3 roughHalf = voxel->GetRoughSize() * 0.5f;
	Vector3 voxMin = voxPos - roughHalf;
	Vector3 voxMax = voxPos + roughHalf;
	//`````````````````````````
#pragma endregion

#pragma region Õ“Ë”»’èij
	//------------------------------------------------------
	// ‡@ ‘e”»’èFLine ‚Ì AABB vs Voxel RoughAABB
	//------------------------------------------------------

	// ‘e”»’èAABBŒğ·
	if (lineMax.x < voxMin.x || lineMin.x > voxMax.x) { return false; }
	if (lineMax.y < voxMin.y || lineMin.y > voxMax.y) { return false; }
	if (lineMax.z < voxMin.z || lineMin.z > voxMax.z) { return false; }

	//------------------------------------------------------
	// ‡A ×”»’èFƒZƒ‹’PˆÊ‚ÌÚ×ƒ`ƒFƒbƒN
	//------------------------------------------------------
	const float cell = voxel->GetCellSize();

	bool hit = false;
	float maxOverlap = -1.0f;
	Vector3 deepestNormal = line->GetDirection().Normalized();
	int deepestIdx = -1;

	for (const auto& kv : voxel->GetCellWorldPoss()) {
		Vector3 cmin = kv.second - Vector3(cell * 0.5f);
		Vector3 cmax = kv.second + Vector3(cell * 0.5f);

		// ü•ª‚Æ AABB ‚ÌÅ‹ß“_‚ğæ“¾
		Vector3 closest = line->ClosestPointAABB(cmin, cmax);

		// ü•ª‘¤‚ÌÅ‹ß“_‚àæ‚é
		Vector3 closestOnLine = line->ClosestPoint(closest);

		Vector3 diff = closestOnLine - closest;
		float distSq = diff.LengthSq();

		if (distSq <= 0.000001f) {
			// ƒR[ƒi[‚ÉŠ®‘Sˆê’v‚Ìê‡
			diff = -line->GetDirection().Normalized();
			distSq = 0.0f;
		}

		if (distSq <= 0.0001f) {
			hit = true;
			voxel->AddHitCellIdx(kv.first);

			float dist = sqrtf(distSq);
			float overlap = (dist > 0.0f) ? -dist : 0.01f;

			if (overlap > maxOverlap) {
				maxOverlap = overlap;

				if (dist > 0.0001f) { deepestNormal = diff / dist; }
				else { deepestNormal = -line->GetDirection().Normalized(); }

				deepestIdx = kv.first;
			}
		}
	}

	if (!hit) { return false; }
#pragma endregion

	//------------------------------------------------------
	// ‡B ‰Ÿ‚µo‚µiLineCollider ‘¤‚¾‚¯‰Ÿ‚·j
	//------------------------------------------------------
	if (line->GetPushFlg() && line->GetDynamicFlg()) {
		Vector3 pushDir = line->GetDirection().Normalized();

		// –@ü•ûŒü‚ğ Line ‚Ì‰Ÿ‚µo‚µ•ûŒü‚Ö“Š‰e‚·‚é
		float dotValue = deepestNormal.Dot(pushDir);

		if (fabs(dotValue) > 1e-5f) {
			Vector3 pushVec = pushDir * (maxOverlap * dotValue);

			line->SetTransformPos(line->GetTransform().pos + pushVec);
		}
	}

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
#pragma region •K—vî•ñ‚ğæ“¾
	// sphere(‹…‘Ì)````````
	// À•W
	Vector3 c = sphere->GetPos();
	// ”¼Œa
	float r = sphere->GetRadius();
	// ``````````````

	// box(ƒ{ƒbƒNƒXŒ`ó)````````
	// À•W
	Vector3 boxPos = box->GetPos();
	// ‘å‚«‚³i”¼•ªj
	Vector3 half = box->GetSize() / 2;
	//`````````````````
#pragma endregion

#pragma region Õ“Ë”»’èisphere(‹…‘Ì)‚©‚ç‚İ‚ÄÅ‚à‹ß‚¢‚P“_‚ğ‹‚ß‚Ä ‚»‚±‚Ü‚Å‚Ì‹——£‚ğ‚Í‚©‚Á‚Ä –¢Õ“Ë‚È‚çI—¹j
	// Å‚à‹ß‚¢“_‚ğ‹‚ß‚é
	Vector3 nearest;
	nearest.x = std::clamp(c.x, boxPos.x - half.x, boxPos.x + half.x);
	nearest.y = std::clamp(c.y, boxPos.y - half.y, boxPos.y + half.y);
	nearest.z = std::clamp(c.z, boxPos.z - half.z, boxPos.z + half.z);

	Vector3 normal = c - nearest;
	float distSq = normal.LengthSq();

	if (distSq > r * r) { return false; }
#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	// ‰Ÿ‚µo‚µ‚ª•K—v‚©‚Ç‚¤‚©
	if (NeedPush(sphere, box)) {

		// Õ“Ë”»’èæ“¾‚µ‚½dispSq‚ğg‚Á‚ÄAÀÛ‚Ì‹——£‚ğZo‚·‚é
		float dist = sqrtf(distSq);

		if (dist > 0.0001f) {
			// Š®‘Sˆê’v‚µ‚Ä‚¢‚½‚ç“K“–‚È•ûŒüiˆÚ“®•ûŒü‚Ì‹t•ûŒüj‚ğ—^‚¦‚é
			normal = -sphere->GetTransform().Velocity();
			dist = 0.0f;
		}

		// ‚ß‚è‚ñ‚¾—Ê
		float overlap = r - dist;

		// ‰Ÿ‚µo‚µˆ—
		ApplyPush(sphere, box, normal.Normalized(), overlap);
	}
#pragma endregion

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
	return false;
}

bool CollisionManager::ModelToVoxel(ModelCollider* model, VoxelCollider* voxel)
{
	return false;
}