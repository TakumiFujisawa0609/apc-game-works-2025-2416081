#pragma once

#include"../CharactorBase.h"

#include"Attack/Fall/FallManager.h"
#include"Attack/Stone/StoneShooter.h"
#include"Attack/PsychoRock/PsychoRockShooter.h"
#include"Attack/RockWall/RockWallShooter.h"

#include"UI/HpBar/BossHpBarManager.h"
#include"UI/Preview/BossPreview.h"

#include"../../../Application/Application.h"

class Boss : public CharactorBase
{
public:

	Boss(const Vector3& playerPos);
	~Boss()override {}

	// “Ç‚İ‚İˆ—
	void Load(void)override;

	// UI‚Ì•`‰æ
	void UiDraw(void)override;

	// “–‚½‚è”»’è‚Ì’Ê’m
	void OnCollision(const ColliderBase& collider)override;

	// Ú’n”»’è‚Ì’Ê’m
	void OnGrounded()override {}

	// ó‘Ô’è‹`
	enum class STATE
	{
		NON,
		IDLE,
		ATTACK,
		DAMAGE,
		STAN,
		BIG_DAMAGE,
		DEATH,
		END,

		MAX
	};


	std::vector<ColliderBase*> GetCollider(void)const {
		std::vector<ColliderBase*>ret = {};

		for (ColliderBase*& collider : ActorBase::GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : fall_->GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : psycho_->GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : stone_->GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : rockWall_->GetCollider()) { ret.emplace_back(collider); }

		return ret;
	}

	// Å‘åHP
	static constexpr int HP_MAX = 200;

private:

#pragma region ’è”’è‹`
	// ƒ‚ƒfƒ‹‚Ì•\¦ƒXƒP[ƒ‹
	const float SCALE = 2.0f;

	// ƒTƒCƒY
	const Vector3 SIZE = Vector3(304.004f, 387.109f, 205.034f) * SCALE;

	// ’†S“_•â³
	const Vector3 CENTER_DIFF = Vector3(0.0f, 190.9405f, 0.0f) * SCALE;

	// ƒ[ƒJƒ‹‰ñ“]Šp“x
	const Vector3 LOCAL_ANGLE = { 0.0f,Deg2Rad(180.0f),0.0f };

	// ƒJƒvƒZƒ‹“–‚½‚è”»’è—p’è”`````````````````````````````````````````````````````````

	// ƒJƒvƒZƒ‹‚Ì”¼Œa
	const float CAPSULE_COLLIDER_RADIUS = SIZE.z / 2;

	// ƒJƒvƒZƒ‹‚Ìƒ[ƒJƒ‹ n“_/I“_ À•W
	const Vector3 CAPSULE_COLLIDER_START_POS = Vector3::Yonly(SIZE.y / 2 - CAPSULE_COLLIDER_RADIUS);
	const Vector3 CAPSULE_COLLIDER_END_POS = Vector3::Yonly(-SIZE.y / 2 + CAPSULE_COLLIDER_RADIUS);

	// ƒJƒvƒZƒ‹‚Ì\•ª—£‚ê‚Ä‚¢‚é‹——£
	const float CAPSULE_COLLIDER_ENOUGH_DISTANCE = (CAPSULE_COLLIDER_START_POS - CAPSULE_COLLIDER_END_POS).Length() + CAPSULE_COLLIDER_RADIUS;

	// `````````````````````````````````````````````````````````````````````

	// ‘¬“x
	const float SPEED = 10.0f;

	// Å‘åƒqƒbƒgƒ|ƒCƒ“ƒg
	const unsigned short MAX_HP = 200;

	// Å‘åƒ}ƒXƒ^[ƒ‰ƒCƒt”
	static constexpr unsigned char MASTER_LIFE = 2;
#pragma endregion

	// ƒqƒbƒgƒ|ƒCƒ“ƒg
	unsigned short hp_;

	// ƒ}ƒXƒ^[ƒ‰ƒCƒt”
	unsigned char masterLife_;

	// ƒvƒŒƒrƒ…[
	BossPreview* preview;

	// ƒvƒŒƒrƒ…[À•W
	const Vector2 PREVIEW_POS = Vector2(App::SCREEN_SIZE_X - BossPreview::SIZE - 10, 10);

	// HPƒo[ŠÇ—ƒNƒ‰ƒX
	BossHpBarManager* hpBar_[MASTER_LIFE];

	// HPƒo[‚ÌF
	const unsigned int HP_BAR_COLOR[MASTER_LIFE] =
	{
		0xeeee00,//2‰ñ–Ú
		0x0000ff,//1‰ñ–Ú
	};

	// HPƒo[‚ÌÀ•W
	const Vector2 HP_BAR_POS = Vector2(PREVIEW_POS.x - BossHpBarManager::HP_BAR_WHOLE_SIZE_X, 10.0f);

	void HpSharpen(int damage);
	void LifeSharpen(void);

	void CharactorInit(void)override;
	void CharactorUpdate(void)override;
	void CharactorDraw(void)override;
	void CharactorAlphaDraw(void)override;
	void CharactorRelease(void)override;

	// ó‘Ô•ÊŠÖ”````
	void Non(void) {};
	void Idle(void);
	void Attack(void);
	void Damage(void);
	void Stan(void);
	void BigDamage(void);
	void Death(void);
	void End(void) {};
	//`````````

#pragma region ó‘Ô•ÊŠÖ”‚Ì’†g

	// UŒ‚ó‘Ô```````````````````

	// ó‘Ô’è‹`
	enum class ATTACK_KINDS
	{
		NON = -1,

		FALL,
		STONE,
		PSYCHO,
		WALL,

		MAX,
	};

	// Œ»İ‚ÌUŒ‚ó‘Ô
	ATTACK_KINDS attackState_;

	// UŒ‚‚Ìí—Ş‚Ì’Š‘I‚ğs‚¤ŠÖ”
	ATTACK_KINDS AttackLottery(void);

	//ŠeUŒ‚‚ÌƒCƒ“ƒXƒ^ƒ“ƒX----
	FallManager* fall_;
	StoneShooter* stone_;
	PsychoRockShooter* psycho_;
	RockWallShooter* rockWall_;
	//------------------------

	// UŒ‚‚ÌŠÔŠu‚ğŠÇ—‚·‚éƒJƒEƒ“ƒ^[
	unsigned short attackInterval_;

	// ŠeUŒ‚‚ÌI—¹Œã‚Ì‘Ò‹@ŠÔ
	const int ATTACK_INTERVAL[(int)ATTACK_KINDS::MAX] =
	{
		150,//FALL
		150,//STONE
		200,//PSYCHO
		100,//WALL
	};

	// UŒ‚‘JˆÚŒã1‰ñ–Ú‚ğŒ©•ª‚¯‚é•Ï”
	bool attackInit_;
	// UŒ‚ŠJn‚ÌuŠÔ‚ğŒ©•ª‚¯‚é•Ï”
	bool attackStart_;
	// UŒ‚I—¹‚ğŒ©•ª‚¯‚é•Ï”
	bool attackEnd_;

	const unsigned char SP_ATTACK_MEASU = 5;
	unsigned char spAttackMeasu_;
	// `````````````````````````

	// UŒ‚ó‘Ô`````````````````````
	const int STAN_TIME = 500;
	int stanTimer_;
	// `````````````````````````


#pragma endregion

#pragma region ƒ‚[ƒVƒ‡ƒ“
	// ƒ‚[ƒVƒ‡ƒ“‚Ì‘S‚Ä
	enum class ANIME_TYPE {
		ATTACK,
		DAMAGE,
		DEATH,
		IDLE,
		JUMP,
		PUNCH,
		RUN,
		SLAP,
		STAN,
		WALK,

		MAX,

		// ƒ‚ƒfƒ‹ŠO
		FALL,
	};

	// ƒ‚ƒfƒ‹‚É•R‚Ã‚¢‚Ä‚¢‚éƒ‚[ƒVƒ‡ƒ“‚ÌƒXƒs[ƒh‚Ì’è‹`
	const float IN_FBX_ANIME_SPEED[(int)ANIME_TYPE::MAX] = {
		1.0f, // ATTACK
		1.0f, // DAMAGE
		1.0f, // DEATH
		1.0f, // IDLE
		1.0f, // JUMP
		1.0f, // PUNCH
		1.0f, // RUN
		1.0f, // SLAP
		0.5f, // STAN
		1.0f, // WALK
	};

	// ƒ‚[ƒVƒ‡ƒ“‚Ì‰Šúİ’è
	void AnimeLoad(void);
#pragma endregion

#pragma region ƒvƒŒƒCƒ„[‚ª•ø‚¦‚é‰ºˆÊƒNƒ‰ƒX‚ÌƒƒCƒ“ˆ—‚ğ‚Ü‚Æ‚ß‚ÄŒÄ‚Ño‚·
	void LowerLoad(void);
	void LowerInit(void);
	void LowerUpdate(void);
	void LowerDraw(void);
	void LowerAlphaDraw(void);
	void LowerRelease(void);
#pragma endregion

	const Vector3& playerPos;
};
