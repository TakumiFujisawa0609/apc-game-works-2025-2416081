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
	~Player()override {};

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

	const float RADIUS = SIZE.z / 2;

	const Vector3 CAPSULE_COLLIDER_START_POS = Vector3::Yonly(SIZE.y / 2 - RADIUS);
	const Vector3 CAPSULE_COLLIDER_END_POS = Vector3::Yonly(RADIUS);

	const Vector3 LINE_COLLIDER_START_POS = Vector3();
	const Vector3 LINE_COLLIDER_END_POS = Vector3::Yonly(-SIZE.y * 0.5f);

#pragma endregion

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

	// Å‘åƒqƒbƒgƒ|ƒCƒ“ƒg
	static constexpr unsigned char HP_MAX = 100;
	// ƒqƒbƒgƒ|ƒCƒ“ƒg
	unsigned char hp;

#pragma region ó‘Ô•ÊŠÖ”‚Ì’†g
	// ˆÚ“®ˆ—ŠÖŒW--------------------------

	// ’è”
	static constexpr float RUN_SPEED = 10.0f;			//ƒvƒŒƒCƒ„[‚Ì‘–‚é‘¬“x
	static constexpr float MAX_JUMP_POWER = 18.0f;		//Å‘åƒWƒƒƒ“ƒv—Í
	static constexpr int INPUT_JUMPKEY_FRAME = 12;		//ƒWƒƒƒ“ƒvƒL[‚ğó‚¯•t‚¯‚éƒtƒŒ[ƒ€”
	static constexpr int JUMP_NUM = 2;					//ƒWƒƒƒ“ƒv‰Â”\‰ñ”

	// ŠÖ”
	void Run(void);			//‰¡ˆÚ“®ŠÖ”
	void Jump(void);		//ƒWƒƒƒ“ƒvŠÖ”

	// •Ï”
	bool isJump[JUMP_NUM];	//ƒWƒƒƒ“ƒv‚µ‚Ä‚¢‚é‚©‚Ì•Ï”
	int jumpKeyCounter[JUMP_NUM];
	//---------------------------------------

	// ƒpƒ“ƒ`````````````
	
	// ’è”
	enum class ATTACK_STAGE { NON = -1, FIRST, SECOND, MAX, };		//UŒ‚‚Ì’i”
	static constexpr int INPUT_ATTACK_FRAME = 20;					//Ÿ‚Ì’i‚É‚Â‚È‚ª‚éŠÔ(ƒtƒŒ[ƒ€”)

	// ŠÖ”
	void AttackMove(void);

	// ƒCƒ“ƒXƒ^ƒ“ƒX
	PlayerPunch* punch;

	// UŒ‚‚Ì’i”
	ATTACK_STAGE attackStage;
	bool isAttack[(int)ATTACK_STAGE::MAX];
	int attackStageCounter;
	// ```````````````

	// ‚¦‚®‚èæ‚é``````````
	
	// ƒCƒ“ƒXƒ^ƒ“ƒX
	PlayerGouge* gouge;
	
	// •Ï”
	
	// Gouge(‚¦‚®‚èæ‚è)‚ğÀs‚µ‚½‚©‚Ç‚¤‚©‚Ìƒtƒ‰ƒO
	bool isGouge;

	//````````````````

	// “Š‚°``````````````
	
	// ƒCƒ“ƒXƒ^ƒ“ƒX
	Throwing* throwing;

	// ŠÖ”
	void CarryRun(void);		//‰¡ˆÚ“®ŠÖ”
	void CarryJump(void);		//ƒWƒƒƒ“ƒvŠÖ”

	//````````````````

	// ƒmƒbƒNƒoƒbƒN‚ÌˆÚ“®ƒxƒNƒgƒ‹
	Vector3 knockBackVec;

#pragma endregion

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

#pragma region ƒ‚[ƒVƒ‡ƒ“
	// ƒ‚[ƒVƒ‡ƒ“‚Ì‘S‚Ä
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
	// ƒ‚[ƒVƒ‡ƒ“‚ÌÄ¶ƒXƒs[ƒh”z—ñ
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
		2.0f,	//GOUPE
		1.0f,	//CARRY_IDLE
		2.5f,	//CARRY_RUN
		1.0f,	//THROW
		1.0f,	//DAMAGE
		1.0f,	//DEATH
	};

	void HpSharpen(int damage);
#pragma endregion

#pragma region ƒvƒŒƒCƒ„[‚ª•ø‚¦‚é‰ºˆÊƒNƒ‰ƒX‚ÌƒƒCƒ“ˆ—‚ğ‚Ü‚Æ‚ß‚ÄŒÄ‚Ño‚·
	void LowerLoad(void);
	void LowerInit(void);
	void LowerUpdate(void);
	void LowerDraw(void);
	void LowerAlphaDraw(void);
	void LowerRelease(void);
#pragma endregion

	// ƒXƒe[ƒW•œŠˆ‚ÉŒÄ‚Ño‚·ŠÖ”ƒ|ƒCƒ“ƒ^
	std::function<void(void)>stageRevival;
};

