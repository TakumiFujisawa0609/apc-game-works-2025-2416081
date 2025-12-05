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

void CollisionManager::Matching(std::vector<ColliderBase*> as, std::vector<ColliderBase*> bs)
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
	Vector3 vec = a->GetPos() - b->GetPos();
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
	if (a->GetPushFlg() && b->GetPushFlg()) {

		// ‚ß‚è‚ñ‚¾—Ê
		float overrap = radius - vec.Length();

		// ƒxƒNƒgƒ‹‚ğ³‹K‰»‚µ‚Ä‚¨‚­
		vec.Normalize();

		// “®“IƒIƒuƒWƒFƒNƒg‚©”Û‚©‚Ìƒtƒ‰ƒO‚ğæ“¾
		bool aDynamic = a->GetDynamicFlg(), bDynamic = b->GetDynamicFlg();

		// —¼•û“®“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		if (aDynamic && bDynamic) {

			// ‚¨Œİ‚¢‚Ìd‚İ‚É‚¨‚¯‚éŠ„‡‚ğŒvZ
			float aWeightRatio = 0.0f, bWeightRatio = 0.0f;
			WeightRatioCalculation(a->GetPushWeight(), b->GetPushWeight(), aWeightRatio, bWeightRatio);

			// ŒvZ‚µ‚½d‚İ‚ÌŠ„‡‚ğ‚ß‚è‚İ—Ê‚É‚©‚¯‡‚í‚¹‚½”’l‚Å‚¨Œİ‚¢‚ğ‰Ÿ‚µo‚·
			a->SetTransformPos(a->GetTransform().pos + (vec * (overrap * aWeightRatio)));
			b->SetTransformPos(b->GetTransform().pos + (-vec * (overrap * bWeightRatio)));
		}
		// a‚¾‚¯“®“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		else if (aDynamic && !bDynamic) {
			a->SetTransformPos(a->GetTransform().pos + (vec * overrap));
		}
		// b‚¾‚¯“®“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		else if (!aDynamic && bDynamic) {
			b->SetTransformPos(b->GetTransform().pos + (-vec * overrap));
		}
		// —¼•ûÃ“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		else { /*‰½‚à‚µ‚È‚¢*/ }
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
	Vector3 vec = pa - pb;
	float distSq = vec.LengthSq();
	float radSum = aRadius + bRadius;

	// 
	if (distSq >= radSum * radSum) { return false; }

#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ

	// ‰Ÿ‚µo‚µ‚ª•K—v‚©‚Ç‚¤‚©
	if (a->GetPushFlg() && b->GetPushFlg())	{

		float dist = std::sqrt(distSq);
		if (dist < 1e-6f) {
			// ƒ[ƒ‹——£
			vec = -a->GetTransform().Velocity().Normalized();
			dist = 0.0f;
		}
		else {
			// ³‹K‰»
			vec /= dist;
		}

		// ‚ß‚è‚İ—Ê
		float overlap = radSum - dist;

		// “®“Iƒtƒ‰ƒO
		bool aDyn = a->GetDynamicFlg();
		bool bDyn = b->GetDynamicFlg();

		// —¼•û“®“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		if (aDyn && bDyn)
		{
			float aRatio = 0.0f, bRatio = 0.0f;
			WeightRatioCalculation(a->GetPushWeight(), b->GetPushWeight(), aRatio, bRatio);

			a->SetTransformPos(a->GetTransform().pos + vec * (overlap * aRatio));
			b->SetTransformPos(b->GetTransform().pos - vec * (overlap * bRatio));
		}
		// A‚¾‚¯“®“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		else if (aDyn && !bDyn)
		{
			a->SetTransformPos(a->GetTransform().pos + vec * overlap);
		}
		// B‚¾‚¯“®‚­“®“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		else if (!aDyn && bDyn)
		{
			b->SetTransformPos(b->GetTransform().pos - vec * overlap);
		}
		// —¼•ûÃ“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		else { /*‰½‚à‚µ‚È‚¢*/ }
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
	Vector3 vec = C - Q;

	// ‹——£‚Ì‚QæiŒvZ—ÊŒyŒ¸‚Ì‚½‚ß‚Qæ‚Åæ“¾jAŒã‚Ù‚Çg‚¤‰Â”\«‚ª‚ ‚é‚Ì‚Åƒ[ƒJƒ‹•Ï”‚É•Û‚µ‚Ä‚¨‚­
	float distSq = vec.LengthSq();

	// ‚¨Œİ‚¢‚Ì”¼Œa‚Ì‡Œv
	float radiusSum = rS + rC;

	// ‹——£‚Ì‚Qæ‚Æ‚¨Œİ‚¢‚Ì”¼Œa‚Ì‡Œv‚Ì‚Qæ‚ğ”ä‚×‚Ä”»’èi–¢Õ“Ë‚È‚çI—¹j
	if (distSq >= radiusSum * radiusSum) { return false; }

	//````````````````````````````````````````````````````````````````
#pragma endregion

#pragma region Õ“ËŠm’èF‰Ÿ‚µo‚µ‚ª•K—v‚©->•K—v‚È‚ç‰Ÿ‚µo‚µ
	// ‰Ÿ‚µo‚µ‚ª•K—v‚©‚Ç‚¤‚©
	if (sphere->GetPushFlg() && capsule->GetPushFlg())
	{
		// Õ“Ë”»’èæ“¾‚µ‚½dispSq‚ğg‚Á‚ÄAÀÛ‚Ì‹——£‚ğZo‚·‚é
		float dist = std::sqrtf(distSq);

		if (dist < 1e-6f) {
			// Š®‘Sˆê’v‚µ‚Ä‚¢‚½‚ç“K“–‚È•ûŒü‚ğ—^‚¦‚é
			vec = -sphere->GetTransform().Velocity();
			dist = 0.0f;
		}

		// ³‹K‰»
		vec.Normalize();

		// ‚ß‚è‚İ—Ê
		float overlap = radiusSum - dist;

		// “®“Iƒtƒ‰ƒO
		bool sDynamic = sphere->GetDynamicFlg();
		bool cDynamic = capsule->GetDynamicFlg();

		// —¼•û“®“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		if (sDynamic && cDynamic)
		{
			float sRatio = 0.0f, cRatio = 0.0f;
			WeightRatioCalculation(sphere->GetPushWeight(), capsule->GetPushWeight(), sRatio, cRatio);

			sphere->SetTransformPos(sphere->GetTransform().pos + vec * (overlap * sRatio));
			capsule->SetTransformPos(capsule->GetTransform().pos - vec * (overlap * cRatio));
		}
		// spherei‹…‘Ìj‚¾‚¯“®“I‚Ìê‡
		else if (sDynamic && !cDynamic)
		{
			sphere->SetTransformPos(sphere->GetTransform().pos + vec * overlap);
		}
		// capsuleiƒJƒvƒZƒ‹j‚¾‚¯“®“I‚Ìê‡
		else if (!sDynamic && cDynamic)
		{
			capsule->SetTransformPos(capsule->GetTransform().pos - vec * overlap);
		}
		// —¼•ûÃ“IƒIƒuƒWƒFƒNƒg‚Ìê‡
		else { /*‰½‚à‚µ‚È‚¢*/ }
	}
#pragma endregion

	// “–‚½‚Á‚½
	return true;
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
