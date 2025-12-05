#include"CollisionManager.h"

#include<cmath>

//std::vector<ColliderBase*> CollisionManager::playerColliders_ = {};
//std::vector<ColliderBase*> CollisionManager::enemyColliders_ = {};
//std::vector<ColliderBase*> CollisionManager::stageColliders_ = {};

void CollisionManager::Add(ColliderBase* collider)
{
	if (!collider) { return; }

	// ƒ^ƒO‚ğŒ©•ª‚¯‚Ä“K‚µ‚½”z—ñ‚ÉŠi”[
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
	// ƒvƒŒƒCƒ„[~ƒXƒe[ƒW
	Matching(playerColliders_, stageColliders_);
	
	// ƒGƒlƒ~[~ƒXƒe[ƒW
	Matching(enemyColliders_, stageColliders_);
	
	// ƒvƒŒƒCƒ„[~ƒGƒlƒ~[
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
	if (aShape == SHAPE::CAPSULE && bShape == SHAPE::VOXEL) { return SphereToVoxel(dynamic_cast<SphereCollider*>(a), dynamic_cast<VoxelCollider*>(b)); }
	if (aShape == SHAPE::VOXEL && bShape == SHAPE::CAPSULE) { return SphereToVoxel(dynamic_cast<SphereCollider*>(b), dynamic_cast<VoxelCollider*>(a)); }

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

#pragma region Õ“Ë”»’èi‚Q“_ŠÔ‚Ì‹——£‚ğ‚Í‚©‚Á‚ÄA–¢Õ“Ë‚È‚çI—¹j
	// ƒxƒNƒgƒ‹‚Ì’·‚³‚Ì‚Qæ‚Æ”¼Œa‚Ì‡Œv‚Ì‚Qæ‚ğ”ä‚×‚Ä”»’èi–¢Õ“Ë‚È‚çI—¹j
	//if (vec.LengthSq() > radius * radius) { return false; }

	// Sphere ~ Sphere ‚Ìê‡A‚·‚Å‚É”»’è‚ÍÏ‚ñ‚Å‚¢‚é‚Ì‚Å“–‚½‚Á‚Ä‚¢‚é‘O’ñ‚Åi‚ß‚é
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

#pragma region Õ“Ë”»’èi‚¨Œİ‚¢‚Ìü•ªã‚É‚¨‚¯‚éÅ‹ß“_‚ğ‹‚ß‚Ä‚»‚Ì‚Q“_ŠÔ‚Ì‹——£‚ğ‚Í‚©‚Á‚ÄA–¢Õ“Ë‚È‚çI—¹j
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
	Vector3 spherePos = sphere->GetPos();
	float radius = sphere->GetRadius();
	// ``````````````````
#pragma endregion

#pragma region Õ“Ë”»’èij
	// Å‹ß“_-spherei‹…‘Ìj’†SÀ•W‚Æ‚ÌƒxƒNƒgƒ‹‚ğ‚Æ‚é
	Vector3 normal = line->ClosestPoint(spherePos) - spherePos;

	// ƒxƒNƒgƒ‹‚Ì‹——£‚Ì‚Qæ‚ğZo‚·‚é
	float distSq = normal.LengthSq();

	// ‹——£‚Ì‚Qæ‚Æspherei‹…‘Ìj‚Ì”¼Œa‚Ì‚Qæ‚ğ”ä‚×‚Ä”»’è
	if (distSq > radius * radius) { return false; }
#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	// ‰Ÿ‚µo‚µ‚ª•K—v‚©‚Ç‚¤‚©
	if (NeedPush(line, sphere)) {
		// ‰Ÿ‚µo‚µ•ûŒüFLine‚Ì•ûŒü‚É‹­§ŒÅ’è
		Vector3 pushDir = line->GetDirection();

		// Õ“Ë”»’èæ“¾‚µ‚½dispSq‚ğg‚Á‚ÄAÀÛ‚Ì‹——£‚ğZo‚·‚é
		float dist = sqrtf(distSq);

		// ‚ß‚è‚ñ‚¾—Ê
		float overlap = radius - dist;

		// ‰Ÿ‚µ–ß‚µˆ—
		ApplyPush(line, sphere, normal.Normalized(), overlap);
	}
#pragma endregion

	// “–‚½‚Á‚½
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
	return false;
}

bool CollisionManager::LineToModel(LineCollider* line, ModelCollider* model)
{
	return false;
}

bool CollisionManager::LineToVoxel(LineCollider* line, VoxelCollider* voxel)
{
	// ”»’è‚ª•¡G‚É‚È‚é‚Ì‚ÅA”»’f‚·‚é’è‹`‚ğ‚µ‚Ä‚¨‚­
	bool ret = false;
#pragma region •K—vî•ñ‚ğæ“¾
	// lineiü•ªj``````````````````````````````````
	// n“_/I“_
	const Vector3 lineStartPos = line->GetStartPos(), lineEndPos = line->GetEndPos();
	// ü•ª‚Ì”¼•ª‚Ì’·‚³
	const float lineHalfLen = line->GetHalfLen();
	// ````````````````````````````````````````

	// voxeliƒ{ƒNƒZƒ‹j````````````````
	// À•W
	const Vector3 voxelPos = voxel->GetPos();
	// ‘S‘Ì‚ğˆÍ‚ß‚é‘å‚Ü‚©‚ÈƒTƒCƒY
	const Vector3 roughlySize = voxel->GetRoughSize();
	//`````````````````````````
#pragma endregion

#pragma region Õ“Ë”»’èij

#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ

#pragma endregion

	return ret;
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

#pragma region Õ“Ë”»’èispherei‹…‘Ìj‚Ì’†SÀ•W‚©‚çcapsuleiƒJƒvƒZƒ‹jü•ªã‚É‚¨‚¯‚éÅ‹ß“_‚Ü‚Å‚Ì‹——£‚ğ‚Í‚©‚Á‚ÄA–¢Õ“Ë‚È‚çI—¹j
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

#pragma region Õ“Ë”»’èisphere(‹…‘Ì)‚©‚ç‚İ‚ÄÅ‚à‹ß‚¢‚P“_‚ğ‹‚ß‚ÄA‚»‚±‚Ü‚Å‚Ì‹——£‚ğ‚Í‚©‚Á‚ÄA–¢Õ“Ë‚È‚çI—¹j
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
	return false;
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