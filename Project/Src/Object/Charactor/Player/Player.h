#pragma once

#include <functional>

#include"../CharactorBase.h"

#include"Punch/PlayerPunch.h"
#include"Gouge/PlayerGouge.h"
#include"Throwing/Throwing.h"

#include"UI/HpBar/PlayerHpBarManager.h"
#include"UI/Preview/PlayerPreview.h"
#include"UI/OperationUI/PlayerOperationUI.h"

#include"../../../Application/Application.h"

class Player : public CharactorBase
{
public:
	Player();
	~Player()override = default;

	void Load(void)override;
	void UiDraw(void)override;

	void OnCollision(const ColliderBase& collider)override;
	void OnGrounded()override;


	enum class STATE
	{
		NON,
		MOVE,
		ATTACK,
		GOUGE,
		CARRY_OBJ,
		THROWING_OBJ,
		EVASION,
		DAMAGE,
		DEATH,
		END,

		MAX
	};

	std::vector<ColliderBase*> GetCollider(void)const override { 
		std::vector<ColliderBase*>ret = {};

		for (ColliderBase*& collider : ActorBase::GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : punch->GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : gouge->GetCollider()) { ret.emplace_back(collider); }
		for (ColliderBase*& collider : throwing->GetCollider()) { ret.emplace_back(collider); }

		return ret;
	}

	void SetStageRevivalFunc(std::function<void(void)>ptr) { stageRevival = std::move(ptr); }

	// Œ»İ‚ÌHP‚Ìc‚èŠ„‡‚ğ•Ô‚·
	float HpRatio(void)const { return ((float)hp / (float)HP_MAX); }

	// UŒ‚‚Ìí—Ş
	enum class ATTACK_DAMAGE_TYPE
	{
		PUNCH,
		THROWING_ROCK,

		MAX
	};

	// UŒ‚‚²‚Æ‚Ìƒ_ƒ[ƒW—Êƒe[ƒuƒ‹
	static constexpr unsigned char ATTACK_DAMAGE_TABLE[(int)ATTACK_DAMAGE_TYPE::MAX] = {
		5,	//PUNCH
		30,	//THROWING_ROCK
	};

private:

#pragma region ’è”’è‹`
	// ƒ‚ƒfƒ‹ƒXƒP[ƒ‹
	const float SCALE = 0.1f;

	// ƒ‚ƒfƒ‹‚ÌƒTƒCƒY
	const Vector3 SIZE = Vector3(1785, 1600, 700) * SCALE;

	// ƒ‚ƒfƒ‹‚Ì’†S“_‚ÌƒYƒŒ
	const Vector3 CENTER_DIFF = Vector3(0, -860, 0) * SCALE;

	// ƒ‚ƒfƒ‹‚ÌŠp“x‚ÌƒYƒŒ
	const Vector3 LOCAL_ROT = Vector3(0.0f, Deg2Rad(180.0f), 0.0f);

	// “–‚½‚è”»’èî•ñ````````````````````````````````

	// ƒJƒvƒZƒ‹ƒRƒ‰ƒCƒ_[‚Ì”¼Œa
	const float RADIUS = SIZE.z / 2;
	// ƒJƒvƒZƒ‹ƒRƒ‰ƒCƒ_[‚Ìƒ[ƒJƒ‹n“_
	const Vector3 CAPSULE_COLLIDER_START_POS = Vector3::Yonly(SIZE.y / 2 - RADIUS);
	// ƒJƒvƒZƒ‹ƒRƒ‰ƒCƒ_[‚Ìƒ[ƒJƒ‹I“_
	const Vector3 CAPSULE_COLLIDER_END_POS = Vector3::Yonly(RADIUS);

	// ü•ªƒRƒ‰ƒCƒ_[‚Ìƒ[ƒJƒ‹n“_
	const Vector3 LINE_COLLIDER_START_POS = Vector3();
	// ü•ªƒRƒ‰ƒCƒ_[‚Ìƒ[ƒJƒ‹I“_
	const Vector3 LINE_COLLIDER_END_POS = Vector3::Yonly(-SIZE.y * 0.5f);

	// ```````````````````````````````````````

	// Å‘åƒqƒbƒgƒ|ƒCƒ“ƒg
	const unsigned char HP_MAX = 100;

	// ˆÚ“®ˆ—```````````````````````````

	//ƒvƒŒƒCƒ„[‚Ì‘–‚é‘¬“x
	const float RUN_SPEED = 10.0f;

	//Å‘åƒWƒƒƒ“ƒv—Í
	const float MAX_JUMP_POWER = 18.0f;

	//ƒWƒƒƒ“ƒvƒL[‚ğó‚¯•t‚¯‚éƒtƒŒ[ƒ€”
	const int INPUT_JUMPKEY_FRAME = 12;

	//ƒWƒƒƒ“ƒv‰Â”\‰ñ”
	static constexpr int JUMP_NUM = 2;

	// ```````````````````````````````

	// ƒpƒ“ƒ`ˆ—``````````````````````````

	// UŒ‚’†‚ÌˆÚ“®‘¬“x
	const float ATTACK_MOVE_SPEED = 13.0f;

	// UŒ‚‚Ì’i”
	enum class ATTACK_STAGE { NON = -1, FIRST, SECOND, MAX, };
	// Ÿ‚Ì’i‚É‚Â‚È‚ª‚éŠÔ(ƒtƒŒ[ƒ€”)
	const int INPUT_ATTACK_FRAME = 20;

	// ƒpƒ“ƒ`‚Ì“–‚½‚è”»’è‚Ì”­¶ŠJnŠÔ(ƒAƒjƒ[ƒVƒ‡ƒ“‚ÌÄ¶Š„‡)
	const float PUNCH_COLLIDER_START_RATE = 0.5f;
	// ƒpƒ“ƒ`‚Ì“–‚½‚è”»’è‚Ì”­¶I—¹ŠÔ(ƒAƒjƒ[ƒVƒ‡ƒ“‚ÌÄ¶Š„‡)
	const float PUNCH_COLLIDER_END_RATE = 0.6f;


	// ```````````````````````````````

	// ‰ñ”ğˆ—```````````````````````````

	// ƒvƒŒƒCƒ„[‚Ì‰ñ”ğˆÚ“®‚Ì‘¬“x
	const float EVASION_SPEED = 15.0f;

	// ‰ñ”ğ‚Ì–³“G”»’è‚ğ”­¶‚³‚¹‚éƒAƒjƒ[ƒVƒ‡ƒ“‚ÌÄ¶Š„‡i0.0f`1.0fj
	const float EVASION_INVINCIBLE_ANIME_RATE = 0.7f;

	// ```````````````````````````````

	// ’Í‚İˆ—```````````````````````````

	// ’Í‚İ¬Œ÷‚ÌƒAƒjƒ[ƒVƒ‡ƒ“‚Ì‚Ç‚±‚Å‚¦‚®‚èæ‚è‚Ì“–‚½‚è”»’è‚ğ”­¶‚³‚¹‚é‚©(ƒAƒjƒ[ƒVƒ‡ƒ“‚ÌÄ¶Š„‡)
	const float GOUGE_COLLIDER_START_RATE = 0.33f;

	// ```````````````````````````````

	// ƒIƒuƒWƒFƒNƒg‚ğ’Í‚ñ‚Å‚¢‚é‚Æ‚«‚Ìˆ—``````````````

	// ƒIƒuƒWƒFƒNƒg‚ğ’Í‚ñ‚Å‚¢‚é‚Æ‚«‚ÌˆÚ“®‘¬“x
	const float CARRY_MOVE_SPEED = 5.0f;

	// ```````````````````````````````

	// “Š±ˆ—```````````````````````````

	// “Š±ƒIƒuƒWƒFƒNƒg‚ğ•ú‚ÂuŠÔ‚ÌƒAƒjƒ[ƒVƒ‡ƒ“‚ÌÄ¶Š„‡
	const float THROWING_RELEASE_ANIME_RATE = 0.25f;

	// ```````````````````````````````

	// ƒ_ƒ[ƒWˆ—`````````````````````````

	// ƒ_ƒ[ƒW‚ğó‚¯‚½‚Æ‚«‚Ì–³“GŠÔiƒtƒŒ[ƒ€”j
	const int DAMAGE_INVINCIBLE_FRAME = 60;
	
	// ```````````````````````````````

#pragma endregion

	// ƒqƒbƒgƒ|ƒCƒ“ƒg
	unsigned char hp;

	void CharactorInit(void)override;
	void CharactorUpdate(void)override;
	void CharactorDraw(void)override;
	void CharactorAlphaDraw(void)override;
	void CharactorRelease(void)override;

#pragma region ó‘ÔŠÇ—
	/// <summary>
	/// ó‘Ô‚²‚Æ‚É‘JˆÚ‰Â”\‚ÌSTATE‚ğU‚è•ª‚¯‚Ä“ü—Í‘€ì‚É‰‚¶‚Ä‚»‚ÌSTATE‚É‘JˆÚ‚³‚¹‚é
	/// </summary>
	void StateManager(void);

	// Šeó‘Ô‚Ì‘JˆÚ‚·‚éğŒ------------------------
	void DoStateMove(void);		// ˆÚ“®ó‘Ô‚É‘JˆÚ‚·‚éğŒ
	void DoStateAttack(void);	// UŒ‚ó‘Ô‚É‘JˆÚ‚·‚éğŒ
	void DoStateGouge(void);	// P‚èó‘Ô‚É‘JˆÚ‚·‚éğŒ
	void DoStateThrowing(void);	// “Š‚°ó‘Ô‚É‘JˆÚ‚·‚éğŒ
	void DoStateEvasion(void);	// ‰ñ”ğó‘Ô‚É‘JˆÚ‚·‚éğŒ
	//---------------------------------------------

	// ó‘Ô•ÊŠÖ”````
	void Non(void) {};
	void Move(void);
	void Attack(void);
	void Gouge(void);
	void CarryObj(void);
	void ThrowingObj(void);
	void Evasion(void);
	void Damage(void);
	void Death(void);
	void End(void) {};
	//`````````
#pragma endregion


#pragma region ó‘Ô•ÊŠÖ”‚Ì’†g
	//‰¡ˆÚ“®ŠÖ”
	void Run(void);
	//ƒWƒƒƒ“ƒvŠÖ”
	void Jump(void);

	//ƒWƒƒƒ“ƒv‚µ‚Ä‚¢‚é‚©‚Ìƒtƒ‰ƒO
	bool isJump[JUMP_NUM];
	//ƒWƒƒƒ“ƒvƒL[‚ğó‚¯•t‚¯‚éƒtƒŒ[ƒ€”‚ÌƒJƒEƒ“ƒ^[
	int jumpKeyCounter[JUMP_NUM];
	

	// ƒpƒ“ƒ`’†‚Ì‰ñ“]
	void AttackRotate(void);

	// ƒpƒ“ƒ`‚Ì“–‚½‚è”»’è‚ÌŠÇ—‚ğs‚¤ƒNƒ‰ƒX‚ÌƒCƒ“ƒXƒ^ƒ“ƒX
	PlayerPunch* punch;

	// Œ»İ‚ÌUŒ‚‚Ì’i”
	ATTACK_STAGE attackStage;

	// ’i”‚²‚Æ‚ÌUŒ‚’†‚©‚Ç‚¤‚©‚Ìƒtƒ‰ƒO‚Æ’i”‚ÌƒJƒEƒ“ƒ^[
	bool isAttack[(int)ATTACK_STAGE::MAX];

	// ’i”‚²‚Æ‚ÌŸ‚Ì’i‚É‚Â‚È‚ª‚éŠÔ‚ÌƒJƒEƒ“ƒ^[
	int attackStageCounter;


	// ‚¦‚®‚èæ‚è‚ğs‚¤ƒNƒ‰ƒX‚ÌƒCƒ“ƒXƒ^ƒ“ƒX
	PlayerGouge* gouge;
	
	// Gouge(‚¦‚®‚èæ‚è)‚ğÀs‚µ‚½‚©‚Ç‚¤‚©‚Ìƒtƒ‰ƒO
	bool isGouge;


	// “Š±ƒIƒuƒWƒFƒNƒg‚ğŠÇ—‚·‚éƒNƒ‰ƒX‚ÌƒCƒ“ƒXƒ^ƒ“ƒX
	Throwing* throwing;

	// ƒIƒuƒWƒFƒNƒg‚ğ•ø‚¦‚Ä‚¢‚é‚Æ‚«‚ÌˆÚ“®ˆ—
	void CarryRun(void);


	// ƒmƒbƒNƒoƒbƒN‚ÌˆÚ“®ƒxƒNƒgƒ‹
	Vector3 knockBackVec;

#pragma endregion

	// HP‚ğŒ¸‚ç‚·
	void HpSharpen(int damage);

#pragma region UI
	// ƒvƒŒƒCƒ„[ƒvƒŒƒrƒ…[``````
	// ƒCƒ“ƒXƒ^ƒ“ƒX
	PlayerPreview* preview;

	// À•W
	const Vector2 PREVIEW_POS = Vector2(10, App::SCREEN_SIZE_Y - PlayerPreview::SIZE - 10);
	// ````````````````

	// HPƒo[````````````
	// ƒCƒ“ƒXƒ^ƒ“ƒX
	PlayerHpBarManager* hpBar;

	// À•W
	const Vector2 HP_BAR_POS = Vector2(PREVIEW_POS.x + PlayerPreview::SIZE, PREVIEW_POS.y + (PlayerPreview::SIZE / 2));
	// ```````````````

	// ‘€ìà–¾`````````````
	// ƒCƒ“ƒXƒ^ƒ“ƒX
	PlayerOperationUI* operationUi;

	// À•W
	const Vector2 OPERATION_UI_POS = Vector2(HP_BAR_POS.x, HP_BAR_POS.y - PlayerOperationUI::WHOLE_SIZE_Y);
	// `````````````````
#pragma endregion

#pragma region ƒAƒjƒ[ƒVƒ‡ƒ“
	// ƒAƒjƒ[ƒVƒ‡ƒ“‚Ì‘S‚Ä
	enum class ANIME_TYPE {

		IDLE,
		RUN,
		JUMP,
		FALL,
		EVASION,
		PUNCH_FIRST,
		PUNCH_SECOND,
		GOUGE_IDLE,
		CATCH,
		GOUGE,
		CARRY_IDLE,
		CARRY_RUN,
		THROW,
		DAMAGE,
		DEATH,

		MAX,
	};
	// ƒAƒjƒ[ƒVƒ‡ƒ“‚ÌÄ¶ƒXƒs[ƒhƒe[ƒuƒ‹
	const float INFBX_ANIME_SPEED[(int)ANIME_TYPE::MAX] =
	{
		0.5f,	//IDLE
		6.0f,	//RUN
		5.0f,	//JUMP
		1.5f,	//FALL
		1.5f,	//EVASION
		1.5f,	//PUNCH_FIRST
		1.5f,	//PUNCH_SECOND
		1.0f,	//GOUGE_IDLE
		1.5f,	//CATCH
		3.0f,	//GOUPE
		1.0f,	//CARRY_IDLE
		2.5f,	//CARRY_RUN
		1.0f,	//THROW
		1.0f,	//DAMAGE
		1.0f,	//DEATH
	};

#pragma endregion

#pragma region ƒvƒŒƒCƒ„[‚ª•ø‚¦‚é‰ºˆÊƒNƒ‰ƒX‚ÌƒƒCƒ“ˆ—‚ğ‚Ü‚Æ‚ß‚ÄŒÄ‚Ño‚·
	void LowerLoad(void);
	void LowerInit(void);
	void LowerUpdate(void);
	void LowerDraw(void);
	void LowerAlphaDraw(void);
	void LowerRelease(void);
#pragma endregion


	// “ü—ÍƒxƒNƒgƒ‹‚ğXZ•½–Ê‚Åæ“¾i¶ƒXƒeƒBƒbƒN->ŒÂ•Ê“o˜^ƒL[j‚Ì—Dæ‡ˆÊ
	Vector3 GetInputVec(void)const;

	// ƒXƒe[ƒW•œŠˆ‚ÉŒÄ‚Ño‚·ŠÖ”ƒ|ƒCƒ“ƒ^
	std::function<void(void)>stageRevival;
};

