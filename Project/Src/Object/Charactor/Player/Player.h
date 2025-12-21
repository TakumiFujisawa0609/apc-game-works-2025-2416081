#pragma once

#include <functional>

#include"../CharactorBase.h"

#include"Punch/PlayerPunch.h"
#include"Gouge/PlayerGouge.h"
#include"Throwing/Throwing.h"

class Player : public CharactorBase
{
public:
	//static constexpr float SCALE = 2.5f;
	//static constexpr VECTOR SIZE = { 64.0f,180.0f,35.0f };
	//const Vector3 SIZE = Vector3(25.0f, 70.0f, 35.0f) * SCALE;
	//const Vector3 CENTER_DIFF = Vector3(0.0f, ((SIZE.y / 2) * SCALE), 15.0f) * SCALE;

	Player(const Vector3& cameraAngle);
	~Player()override {};

	void Load(void)override;
	void UiDraw(void);

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

		MAX
	};

	static constexpr int HP_MAX = 100;

	// ˆÚ“®ó‘Ô‚Åg—p‚·‚é``-------------------------------------------------------------------
	// ’è”
	static constexpr float RUN_SPEED = 10.0f;			//ƒvƒŒƒCƒ„[‚Ì‘–‚é‘¬“x

	static constexpr float MAX_JUMP_POWER = 100.0f;		//Å‘åƒWƒƒƒ“ƒv—Í
	static constexpr int INPUT_JUMPKEY_FRAME = 6;		//ƒWƒƒƒ“ƒvƒL[‚ğó‚¯•t‚¯‚éƒtƒŒ[ƒ€”
	static constexpr int JUMP_NUM = 2;					//ƒWƒƒƒ“ƒv‰Â”\‰ñ”
	//----------------------------------------------------------------------------------------------

	// UŒ‚ó‘Ô‚Åg—p‚·‚é``-------------------------------------------------------------------
	// ’è”
	enum class ATTACK_STAGE { NON = -1, FIRST, SECOND, MAX, };		//UŒ‚‚Ì’i”
	static constexpr int INPUT_ATTACK_FRAME = 20;					//Ÿ‚Ì’i‚É‚Â‚È‚ª‚éŠÔ(ƒtƒŒ[ƒ€”)

	// ŠÖ”
	//----------------------------------------------------------------------------------------------

	// ƒvƒŒƒCƒ„[‚ª•ø‚¦‚é‰ºˆÊƒNƒ‰ƒX‚ÌƒCƒ“ƒXƒ^ƒ“ƒX‚ğ”z—ñ‚ÉŠi”[‚µ‚Ä•Ô‚·
	std::vector<ActorBase*>GetSubIns(void);

	void SetStageRevivalFunc(std::function<void(void)>ptr) { stageRevival_ = std::move(ptr); }

private:

#pragma region ’è”’è‹`
	// ƒXƒP[ƒ‹
	const float SCALE = 2.5f;

	// ƒ‚ƒfƒ‹‚ÌƒTƒCƒY
	const Vector3 SIZE = Vector3(73.204f, 73.096f, 32.071f) * SCALE;

	// ƒ‚ƒfƒ‹‚Ì’†S“_‚ÌƒYƒŒ
	const Vector3 CENTER_DIFF = Vector3(0, -37, 0) * SCALE;

	// ƒ‚ƒfƒ‹‚ÌŠp“x‚ÌƒYƒŒ
	const Vector3 LOCAL_ROT = Vector3(0.0f, Utility::Deg2RadF(180.0f), 0.0f);

	const float RADIUS = SIZE.z;

	const Vector3 CAPSULE_COLLIDER_START_POS = Vector3(0.0f, SIZE.y / 2 - RADIUS, 0.0f);
	const Vector3 CAPSULE_COLLIDER_END_POS = Vector3(0.0f, 0.0f, 0.0f);

	const Vector3 LINE_COLLIDER_START_POS = Vector3(0.0f, 0.0f, 0.0f);
	const Vector3 LINE_COLLIDER_END_POS = Vector3(0.0f, -SIZE.y / 2, 0.0f);
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
	//`````````

#pragma endregion

	//const VECTOR LOCAL_ROT = { 0.0f,Utility::Deg2RadF(180.0f),0.0f };

	int hp_;

#pragma region ó‘Ô•ÊŠÖ”‚Ì’†g
	// ˆÚ“®ˆ—ŠÖŒW--------------------------

	//enum class DIRECTION { NON, FRONT, BACK };
	//struct DIR_VEC { DIRECTION x = DIRECTION::NON, y = DIRECTION::NON, z = DIRECTION::NON; };

	// ŠÖ”
	void Run(void);			//‰¡ˆÚ“®ŠÖ”
	void Jump(void);		//ƒWƒƒƒ“ƒvŠÖ”

	// •Ï”
	const Vector3& cameraAngle_;	//ƒJƒƒ‰‚ÌŠp“x

	bool isJump_[JUMP_NUM];	//ƒWƒƒƒ“ƒv‚µ‚Ä‚¢‚é‚©‚Ì•Ï”
	int jumpKeyCounter_[JUMP_NUM];
	//---------------------------------------

	// UŒ‚ó‘ÔŠÖŒW``````````

	// ŠÖ”
	void AttackMove(void);

	// ƒCƒ“ƒXƒ^ƒ“ƒX
	PlayerPunch* punch_;
	// UŒ‚‚Ì’i”
	ATTACK_STAGE attackStage_;
	bool isAttack_[(int)ATTACK_STAGE::MAX];
	int attackStageCounter_;
	//```````````````

	PlayerGouge* gouge_;

	// “Š‚°``````````````
	Throwing* throwing_;

	// ŠÖ”
	void CarryRun(void);			//‰¡ˆÚ“®ŠÖ”
	void CarryJump(void);		//ƒWƒƒƒ“ƒvŠÖ”
	//````````````````


	Vector3 knockBackVec_;

#pragma endregion

#pragma region ƒ‚[ƒVƒ‡ƒ“
	// ƒ‚[ƒVƒ‡ƒ“‚Ì‘S‚Ä
	enum class ANIME_TYPE {
		IDLE,
		RUN,
		JUMP_POST,
		JUMP,
		FALL,
		EVASION,
		PUNCH_FIRST,
		PUNCH_SECOND,
		PUNCH_THIRD,
		GOUPE,
		CARRY_IDLE,
		CARRY_RUN,
		THROW,
		DAMAGE,
		DEATH,
	};

	// ƒ‚[ƒVƒ‡ƒ“‚Ì‰Šúİ’è
	void AnimeLoad(void);

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
	std::function<void(void)>stageRevival_;
};

