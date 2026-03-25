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
	~Boss()override = default;

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
		NON,		// ó‘Ô‚È‚µ
		IDLE,		// ‘Ò‹@
		ATTACK,		// UŒ‚
		DAMAGE,		// ƒ_ƒ[ƒW
		STAN,		// ƒXƒ^ƒ“
		BIG_DAMAGE,	// ‘åƒ_ƒ[ƒW
		DEATH,		// €–S
		END,		// I—¹

		MAX
	};


	std::vector<ColliderBase*> GetCollider(void)const {
		std::vector<ColliderBase*>ret = {};

		for (ColliderBase*& collider : ActorBase::GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : fall->GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : psycho->GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : stone->GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : rockWall->GetCollider()) { ret.emplace_back(collider); }

		return ret;
	}

	// UŒ‚‚Ìí—Ş
	enum class ATTACK_DAMAGE_TYPE
	{
		FALL,
		STONE,
		PSYCHO,

		MAX
	};

	// UŒ‚‚²‚Æ‚Ìƒ_ƒ[ƒW—Êƒe[ƒuƒ‹
	static constexpr unsigned char ATTACK_DAMAGE_TABLE[(int)ATTACK_DAMAGE_TYPE::MAX] = {
		10,		// FALL
		10,		// STONE
		10,		// PSYCHO
	};

	// ƒvƒŒƒCƒ„[‚©‚çUŒ‚‚²‚Æ‚ÌUŒ‚—Í‚ğó‚¯æ‚é
	void SetPlayerAttackPowerTable(std::vector<unsigned char> playerAttackPowerTable) { this->playerAttackPowerTable = playerAttackPowerTable; }

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

	// Å‘åƒqƒbƒgƒ|ƒCƒ“ƒg
	const unsigned short HP_MAX = 200;

	// Å‘åƒ‰ƒCƒt”
	static constexpr unsigned char LIFE_MAX = 2;

	// ƒ{ƒX‚Ì‰ŠúÀ•W(“®‚©‚È‚¢‚Ì‚Å‚±‚±‚ÅŒÅ’è)
	const Vector3 INIT_POS = Vector3(1000.0f, 300.0f, 1000.0f);

	// Še–³“GŠÔ``````````````````

	// ƒ‰ƒCƒt‚ğ¸‚Á‚½‚Æ‚«‚Ì–³“GŠÔ
	const char LIFE_LOST_INVINCIBLE_TIME = 80;
	// “Š±UŒ‚‚ğó‚¯‚½‚Æ‚«‚Ì–³“GŠÔ
	const char THROWING_DAMAGE_INVINCIBLE_TIME = 60;
	// ƒpƒ“ƒ`UŒ‚‚ğó‚¯‚½‚Æ‚«‚Ì–³“GŠÔ
	const char PUNCH_DAMAGE_INVINCIBLE_TIME = 20;

	// ```````````````````````

	// UŒ‚ó‘Ô`````````````````````````

	enum class ATTACK_KINDS
	{
		NON = -1,

		FALL,
		STONE,
		PSYCHO,
		WALL,

		MAX,
	};

	// ŠeUŒ‚‚Ì’Š‘IŠm—§
	const float ATTACK_LOTTERY_RATE[(int)ATTACK_KINDS::MAX] =
	{
		0.3f,	//FALL
		0.3f,	//STONE
		0.3f,	//PSYCHO
		0.1f,	//WALL
	};

	// ŠeUŒ‚‚ÌI—¹Œã‚Ì‘Ò‹@ŠÔ
	const int ATTACK_INTERVAL[(int)ATTACK_KINDS::MAX] =
	{
		150,	//FALL
		150,	//STONE
		200,	//PSYCHO
		100,	//WALL
	};

	// UŒ‚‚Ì’Š‘I‚ğs‚¤‚½‚ß‚Ì—”¶¬‚ÌÅ‘å’l
	const unsigned short ATTACK_LOTTERY_WORK_VALUE = 10000;

	// `````````````````````````````


	// ƒXƒ^ƒ“ó‘Ô````````````````````````

	// ƒXƒ^ƒ“ó‘Ô‚ÌŒp‘±ŠÔ
	const int STAN_TIME = 500;

	// ƒXƒ^ƒ“ó‘Ô‚©‚ç“|‚³‚ê‚¸‚É•œ‹A‚µ‚½‚Æ‚«‚Ì‰ñ•œ‚·‚é‘Ì—Í‚ÌŠ„‡
	const float STAN_RECOVERY_RATE = 0.2f;

	// `````````````````````````````

	// ‘åƒ_ƒ[ƒWó‘Ô‚ÌŠÔ(ƒAƒjƒ[ƒVƒ‡ƒ“Ä¶Š„‡)
	const float BIG_DAMAGE_TIME = 0.4f;

	// ‘åƒ_ƒ[ƒWó‘Ô‚ÌƒXƒ[‚ÌŠÔiƒtƒŒ[ƒ€”j
	const char BIG_DAMAGE_SLOW_TIME = 20;

	// ƒ‰ƒCƒt‚ª0‚É‚È‚Á‚½‚ÌƒqƒbƒgƒXƒgƒbƒv‚ÌŠÔiƒtƒŒ[ƒ€”j
	const char DEATH_HIT_STOP_TIME = 40;
	// ƒ‰ƒCƒt‚ª0‚É‚È‚Á‚½‚ÌƒXƒ[‚ÌŠÔiƒtƒŒ[ƒ€”j
	const char DEATH_SLOW_TIME = 20;
	// ƒ‰ƒCƒt‚ª0‚É‚È‚Á‚½‚Ì‰æ–Ê—h‚ê‚ÌŠÔiƒtƒŒ[ƒ€”j
	const char DEATH_SCREEN_SHAKE_TIME = 100;

	// ƒvƒŒƒrƒ…[À•W
	const Vector2 PREVIEW_POS = Vector2(App::SCREEN_SIZE_X - BossPreview::SIZE - 10, 10);

	// HPƒo[`````````````````````````````````````````````
	
	// HPƒo[‚ÌF
	const unsigned int HP_BAR_COLOR[LIFE_MAX] =
	{
		0xeeee00,//2‰ñ–Ú
		0x0000ff,//1‰ñ–Ú
	};

	// HPƒo[‚ÌÀ•W
	const Vector2 HP_BAR_POS = Vector2(PREVIEW_POS.x - BossHpBarManager::HP_BAR_WHOLE_SIZE_X, 10.0f);

	// `````````````````````````````````````````````````

	// ƒXƒ^ƒ“ó‘Ô‚ÌHPƒo[‚É•\¦‚·‚éƒeƒLƒXƒg````````````````
	
	// ƒXƒ^ƒ“ó‘ÔHPƒo[‚É•\¦‚·‚éƒeƒLƒXƒg‚Ì“à—e
	const char* HP_BAR_STAN_TEXT = "ƒ`ƒƒƒ“ƒX‚¾I‚Ô‚ñ‰£‚êII";

	// ƒXƒ^ƒ“ó‘ÔHPƒo[‚É•\¦‚·‚éƒeƒLƒXƒg‚ÌÀ•W
	const Vector2I HP_BAR_STAN_TEXT_POS = Vector2I(HP_BAR_POS.x + 60.0f, 25.0f);

	// ƒXƒ^ƒ“ó‘ÔHPƒo[‚É•\¦‚·‚éƒeƒLƒXƒg‚ÌF
	const unsigned int HP_BAR_STAN_TEXT_COLOR = 0xff0000;

	// ƒXƒ^ƒ“ó‘ÔHPƒo[‚É•\¦‚·‚éƒeƒLƒXƒg‚Ì“_–Å‚ÌŠÔŠu
	const char HP_BAR_STAN_TEXT_BLINK_INTERVAL = 15;
	// ```````````````````````````````````

#pragma endregion

	// ƒqƒbƒgƒ|ƒCƒ“ƒg
	unsigned short hp;

	// ƒ‰ƒCƒt
	unsigned char life;

	// ƒvƒŒƒrƒ…[
	BossPreview* preview;

	// HPƒo[ŠÇ—ƒNƒ‰ƒX
	BossHpBarManager* hpBar;

	// ¡ƒXƒ^ƒ“ó‘Ô‚ğ’m‚ç‚¹‚éƒeƒLƒXƒg‚ğ•\¦‚·‚é‚©‚Ç‚¤‚©‚ğ•Ô‚·ŠÖ”i“_–Å‚à‰Á–¡‚µ‚½ÅIŒ‹‰Êj
	bool HpBarStanTextFlg(void) {
		return (
			(state == (int)STATE::STAN) &&
			(((stanTimer / HP_BAR_STAN_TEXT_BLINK_INTERVAL) % 2) == 0)
			);
	}

	// HP‚ğŒ¸­‚³‚¹‚éŠÖ”
	void HpSharpen(int damage);
	// ƒ‰ƒCƒt‚ğŒ¸­‚³‚¹‚éŠÖ”
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

	// Œ»İ‚ÌUŒ‚ó‘Ô
	ATTACK_KINDS attackState;

	// UŒ‚‚Ìí—Ş‚Ì’Š‘I‚ğs‚¤ŠÖ”
	ATTACK_KINDS AttackLottery(void);

	//ŠeUŒ‚‚ÌƒCƒ“ƒXƒ^ƒ“ƒX----
	FallManager* fall;
	StoneShooter* stone;
	PsychoRockShooter* psycho;
	RockWallShooter* rockWall;
	//------------------------

	// UŒ‚‚ÌŠÔŠu‚ğŠÇ—‚·‚éƒJƒEƒ“ƒ^[
	unsigned short attackInterval;

	// UŒ‚‘JˆÚŒã1‰ñ–Ú‚ğŒ©•ª‚¯‚éƒtƒ‰ƒO
	bool attackInit;
	// UŒ‚ŠJn‚ÌuŠÔ‚ğŒ©•ª‚¯‚éƒtƒ‰ƒO
	bool attackStart;
	// UŒ‚I—¹‚ğŒ©•ª‚¯‚éƒtƒ‰ƒO
	bool attackEnd;

	// ```````````````````````

	// ƒXƒ^ƒ“ó‘Ô‚Ìc‚èŠÔ‚ğŠÇ—‚·‚éƒJƒEƒ“ƒ^[
	int stanTimer;

#pragma endregion

#pragma region ƒAƒjƒ[ƒVƒ‡ƒ“
	// ƒAƒjƒ[ƒVƒ‡ƒ“‚Ì‘S‚Ä
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
	};

	// ƒ‚ƒfƒ‹‚É•R‚Ã‚¢‚Ä‚¢‚éƒAƒjƒ[ƒVƒ‡ƒ“‚ÌƒXƒs[ƒh‚Ì’è‹`
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

	// ƒAƒjƒ[ƒVƒ‡ƒ“‚Ì‰Šúİ’è
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

	// ƒvƒŒƒCƒ„[‚ÌÀ•W(QÆ—p)
	const Vector3& playerPos;

	// ƒvƒŒƒCƒ„[‚©‚çUŒ‚‚²‚Æ‚ÌUŒ‚—Í‚ğó‚¯æ‚èŠi”[‚·‚é‚½‚ß‚Ì”z—ñ
	std::vector<unsigned char> playerAttackPowerTable;
};
